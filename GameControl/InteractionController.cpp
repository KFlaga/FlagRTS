#include "InteractionController.h"
#include "SelectionController.h"
#include "HoverController.h"
#include "GuiController.h"
#include "TechnologyController.h"
#include "CameraController.h"
#include <HoverContext.h>
#include <InputManager.h>
#include <Command.h>
#include <CommandButton.h>
#include <OgreRay.h>
#include <HoverTargetTypes.h>
#include <CursorManager.h>
#include <SelectionFlags.h>
#include <Unit.h>
#include <UnitGroup.h>
#include <UnitCommandSet.h>
#include <GameWorld.h>
#include <ConstructionManager.h>
#include <ConstructionPlanner.h>
#include <Player.h>
#include <Resources.h>
#include <Minimap.h>
#include <Map.h>

namespace FlagRTS
{
	InteractionController::InteractionController(Ogre::SceneManager* ogreMgr, 
		SelectionController* selector, GuiController* gui, 
		TechnologyController* tech, CameraController* cam) :
	_hoverCtrl(xNew1(HoverController, ogreMgr)),
		_target(xNew0(CommandTarget)),
		_selectionCtrl(selector),
		_guiCtrl(gui),
		_techCtrl(tech),
		_camCtrl(cam),
		_onCommandChoosen(this),
		_onSelectionChanged(this),
		_onSubgroupChanged(this),
		_onUnitButtonClicked(this),
		_onBuildingButtonClicked(this),
		_onBuildingAvailabilityChanged(this),
		_onMinimapLeftClicked(this),
		_onMinimapLeftDragged(this),
		_onMinimapRightClicked(this),
		_onMinimapRightDragged(this)
	{
		_states.insert(std::make_pair(
			ICStateType::Off, xNew1(ICStateOff,this)));
		_states.insert(std::make_pair(
			ICStateType::SelectionIdle, xNew1(ICStateSelectionIdle,this)));
		_states.insert(std::make_pair(
			ICStateType::SelectionSelecting, xNew1(ICStateSelectionSelecting,this)));
		_states.insert(std::make_pair(
			ICStateType::NoSelectionIdle, xNew1(ICStateNoSelectionIdle,this)));
		_states.insert(std::make_pair(
			ICStateType::NoSelectionSelecting, xNew1(ICStateNoSelectionSelecting,this)));
		_states.insert(std::make_pair(
			ICStateType::CommandArea, xNew1(ICStateCommandArea,this)));
		_states.insert(std::make_pair(
			ICStateType::CommandTarget, xNew1(ICStateCommandTarget,this)));
		_states.insert(std::make_pair(
			ICStateType::ConstructionPlan, xNew1(ICStateConstructionPlan,this)));

		_curState = _states[ICStateType::Off];
		_prevState = _states[ICStateType::Off];

		_selectionCtrl->SelectionChanged() += &_onSelectionChanged;
		_selectionCtrl->SubgroupChanged() += &_onSubgroupChanged;
		_guiCtrl->CommandChoosen() += &_onCommandChoosen;
		_guiCtrl->UnitButtonClicked() += &_onUnitButtonClicked;
		_techCtrl->BuilidngAvailabilityChanged(GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer()) 
			+= &_onBuildingAvailabilityChanged;
		_guiCtrl->BuildingButtonClicked() += &_onBuildingButtonClicked;
		_guiCtrl->MinimapLeftButtonClicked() += &_onMinimapLeftClicked;
		_guiCtrl->MinimapLeftButtonDragged() += &_onMinimapLeftDragged;
		_guiCtrl->MinimapRightButtonClicked() += &_onMinimapRightClicked;
		_guiCtrl->MinimapRightButtonDragged() += &_onMinimapRightDragged;
	}

	InteractionController::~InteractionController()
	{
		_selectionCtrl->SelectionChanged() -= &_onSelectionChanged;
		_selectionCtrl->SubgroupChanged() -= &_onSubgroupChanged;
		_guiCtrl->CommandChoosen() -= &_onCommandChoosen;
		_guiCtrl->UnitButtonClicked() -= &_onUnitButtonClicked;
		xDelete(_hoverCtrl);
		xDelete(_target);
		for(auto stateIt = _states.begin(); stateIt != _states.end(); ++stateIt)
		{
			xDelete(stateIt->Value);
		}
	}

	void InteractionController::BeginInputProcessing()
	{
		ChangeState(InputControllerState::NoSelectionIdle);
		_hoverCtrl->ChangeHoverContextToIdle();
	}

	void InteractionController::Update(const Ogre::Ray& cameraRay)
	{
		// Gui could be handled here too, and if hovers over change
		// input/hover context

		if(_curState->GetStateType() == ICStateType::Off)
			return;

		// Check if mouse hovers over game screen and game is not paused ( in menu )
		// If hovers over gui do not check hover/mouse input, but check if
		// gui controller haven't requested some changes
		if(!_guiCtrl->IsCursorOverGui())
		{
			// If hovers over game map :
			// - check hover target 
			_hoverCtrl->Update(cameraRay, _target);
		}

		// Update state-dependent input, that is command hotkeys, 
		// right/left mouse clicks and mouse movement
		_curState->Update();

		// Reset minimap click states
		_wasMinimapLeftClicked = false;
		_wasMinimapRightClicked = false;

		// Later add:
		// - if number is pressed call UnitGroupsController and select group
		// - if ctrl+number is pressed call UGC and assing selected units to group
	}

	void InteractionController::ChangeStateToIdle()
	{
		if(_selectionCtrl->GetSelectedUnitCount() == 0 ||
			_selectionCtrl->IsNotOwnedUnitSelected())
		{
			ChangeState(ICStateType::NoSelectionIdle);
			_hoverCtrl->ChangeHoverContextToIdle();
		}
		else
		{
			ChangeState(ICStateType::SelectionIdle);
			_hoverCtrl->ChangeHoverContextToUnitGroup(_selectionCtrl->GetSelectedGroupClass());
		}
	}

#pragma region Selection

	void InteractionController::ChangeStateToSelection()
	{
		if(_selectionCtrl->GetSelectedUnitCount() == 0 ||
			_selectionCtrl->IsNotOwnedUnitSelected()) 
			ChangeState(ICStateType::NoSelectionSelecting);
		else
			ChangeState(ICStateType::SelectionSelecting);
		_hoverCtrl->ChangeHoverContextToHold();
	}

	void InteractionController::StartSelection()
	{
		_selectionCtrl->StartSelection(InputManager::Instance()->GetMouseScreenPos());
		ChangeStateToSelection();
	}

	void InteractionController::UpdateSelection()
	{	
		_selectionCtrl->UpdateSelection(InputManager::Instance()->GetMouseScreenPos());
	}

	void InteractionController::EndSelection()
	{
		_selectionCtrl->EndSelection(
			!InputManager::Instance()->CheckModifierIsDown(ModifierCodes::Shift));

		ChangeStateToIdle();
	}

	void InteractionController::AbortSelection()
	{
		_selectionCtrl->AbortSelection();
		ChangeStateToIdle();
	}

	void InteractionController::SelectHoveredUnit()
	{
		_selectionCtrl->SelectHovered(_target,
			!InputManager::Instance()->CheckModifierIsDown(ModifierCodes::Shift));
		AbortSelection();
	}

	void InteractionController::OnSelectionChanged(UnitGroup* selectedGroup)
	{
		// Update hover context + notify gui controller
		if( selectedGroup->GroupCount() == 0)
		{
			_hoverCtrl->ChangeHoverContextToIdle();
		}
		else
		{
			_hoverCtrl->ChangeHoverContextToUnitGroup(
				selectedGroup->GetGroupClass());
		}
		_guiCtrl->UpdateSelectedUnitGroup(selectedGroup);
	}

	void InteractionController::OnContextSubgroupChanged(Array<Unit*>* subgroup)
	{
		ResetContextCommands();
		_guiCtrl->UpdateSelectedUnitContextSubgroup(subgroup);
		// Register subgroup commands if any units are selcted
		// If selection consist of not-owned unit, do not set buttons
		if(subgroup == 0 ||
			_selectionCtrl->IsNotOwnedUnitSelected())
		{
			return;
		}
		UnitCommandSet* commands = _selectionCtrl->GetContextSubgroupCommands();
		for(auto commIt = commands->GetCommandsBegin(), end = commands->GetCommandsEnd();
			commIt != end; ++commIt)
		{
			// For each command with button : register it
			if(commIt->Value.second != 0)
			{
				commIt->Value.first->CheckRequirements();
				RegisterContextCommand(commIt->Value.second, commIt->Value.first);
			}
		}
	}

	void InteractionController::OnUnitButtonClicked(Unit* unit, bool fromContext)
	{
		// Without modifier -> choose this subgroup as context, if not choosen
		// select choosen unit otherwise
		// With shift - deselect choosen unit
		// With ctrl - leave only given type selected
		// With both - delesect unit's type 
		if(InputManager::Instance()->CheckModifierIsDown(ModifierCodes::Shift) &&
			InputManager::Instance()->CheckModifierIsDown(ModifierCodes::Ctrl))
		{
			_selectionCtrl->RemoveUnitTypeFromSelection(unit->GetUnitKind());
		}
		else if(InputManager::Instance()->CheckModifierIsDown(ModifierCodes::Shift))
		{
			_selectionCtrl->DeselectUnit(unit);
		}
		else if(InputManager::Instance()->CheckModifierIsDown(ModifierCodes::Ctrl))
		{
			_selectionCtrl->LeaveOnlyUnitTypeInSelection(unit->GetUnitKind());
		}
		else if(fromContext)
		{
			_selectionCtrl->SelectUnit(unit);
		}
		else
		{
			_selectionCtrl->SetContextSubgroupToType(unit->GetUnitKind());
		}
	}

#pragma endregion

#pragma region Commands

	void InteractionController::RegisterContextCommand(CommandButton* button, Command* command)
	{
		_contextCommands.insert(button, command);
		// Register only not-hidden commands
		if( command->GetAvailability() != TechAvailability::Hidden )
			_guiCtrl->RegisterCommandButton(button);
	}

	void InteractionController::ResetContextCommands()
	{
		_contextCommands.clear();
		_guiCtrl->ResetCommandButtons();
	}

	void InteractionController::CommandChoosen(Command* command)
	{
		_choosenCommand = command;
		switch(command->GetCastType())
		{
		case CommandCastTypes::Instant:
			InstantCommandChoosen();
			break;
		case CommandCastTypes::SingleTarget:
			TargetCommandChoosen();
			break;
		default:
			break;
		}
	}

	void InteractionController::OnCommandChoosen(CommandButton* button)
	{
		_ASSERT(button != 0);
		CommandChoosen(_contextCommands.find(button)->Value);
	}

	bool InteractionController::UpdateContextCommands()
	{
		CheckContextCommandsReqs();
		for(auto commIt = _contextCommands.begin(), end = _contextCommands.end();
			commIt != end; ++commIt)
		{
			// Check only for not hidden commands
			if( commIt->Value->GetAvailability() != TechAvailability::Hidden &&
				InputManager::Instance()->CheckKeyWasPressed(commIt->Key->GetHotkey()))
			{
				CommandChoosen(commIt->Value);
				return true;
			}
		}
		return false;
	}

	void InteractionController::CheckContextCommandsReqs()
	{
		bool commandsAvChanged = false;
		for(auto commIt = _contextCommands.begin(), end = _contextCommands.end();
			commIt != end; ++commIt)
		{
			// First check if some req changed
			TechAvailability oldAv = commIt->Value->GetAvailability();
			commIt->Value->CheckRequirements();
			TechAvailability newAv = commIt->Value->GetAvailability();

			if( oldAv != newAv )
				commandsAvChanged = true;
		}

		if(commandsAvChanged)
		{
			// Reset commands buttons and set new ones
			_guiCtrl->ResetCommandButtons();
			for(auto commIt = _contextCommands.begin(), end = _contextCommands.end();
				commIt != end; ++commIt)
			{
				if( commIt->Value->GetAvailability() != TechAvailability::Hidden )
					_guiCtrl->RegisterCommandButton(commIt->Key);
			}
		}
	}


	void InteractionController::InstantCommandChoosen()
	{
		// Just execute instant one
		OrderChoosenCommand();
	}

	void InteractionController::TargetCommandChoosen()
	{
		// --- if its single target command, then :
		// - single target commands have no command buttons/hotkeys enabled, so
		// reset command buttons
		// - change state to command-target, and hover context to command's one
		_contextCommands.clear();
		_guiCtrl->ResetCommandButtons();
		_hoverCtrl->ChangeHoverContextToCommand(
			_choosenCommand, _selectionCtrl->GetSelectedGroupClass());
		ChangeState(ICStateType::CommandTarget);
	}

	void InteractionController::AbortCommand()
	{
		OnContextSubgroupChanged(_selectionCtrl->GetCurrentContextSubgroup());
		ChangeStateToIdle();
	}

	void InteractionController::OrderChoosenCommand()
	{
		AbortCommand();
		if(_choosenCommand->IsTargetValid(_target))
		{
			_selectionCtrl->OrderSelected(_choosenCommand, _target,
				InputManager::Instance()->CheckModifierIsDown(ModifierCodes::Shift));
		}
		else
		{
			_guiCtrl->ShowQuickNotice(NoticeMessage(
				Notices::GameError, "Invalid target", 
				GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer()));
		}
	}

	ICStateCommandTarget::ICStateCommandTarget(InteractionController* ic) :
		InputControllerState(CommandTarget, ic)
	{ }

	void ICStateCommandTarget::Update()
	{
		// Do not check hotkeys
		// On right pressed abort command and back to Idle state
		// On left accept current target ( regardless if its valid )
		// On esc also abort


		if(InputManager::Instance()->CheckMouseWasPressed(MouseButtons::Right))
		{
			_inputController->AbortCommand();
		}
		else if( _inputController->WasMinimapLeftClicked() /*&& 
			_inputController->GetChoosenCommand()->IsMinimapTargetable()*/ )
		{
			_inputController->OrderChoosenCommand();
		}
		else if(!_inputController->GetGuiController()->IsCursorOverGui())
		{
			if(InputManager::Instance()->CheckMouseWasReleased(MouseButtons::Left))
			{
				_inputController->OrderChoosenCommand();
			}
		}
	}

	ICStateCommandArea::ICStateCommandArea(InteractionController* ic) :
		InputControllerState(CommandArea, ic)
	{ }

	void ICStateCommandArea::Update()
	{
		// Draw area marker defined in input context under mouse
	}

#pragma endregion

#pragma region Construction

	void InteractionController::OnBuildingButtonClicked(TechBuilding* selectedBuilding)
	{
		if( _curState->GetStateType() == ICStateType::ConstructionPlan )
			AbortConstructionPlan();

		_hoverCtrl->ChangeHoverContextToHold();
		StartConstructionPlan(selectedBuilding);
		ChangeState(ICStateType::ConstructionPlan);
	}

	void InteractionController::OnBuildingAvailabilityChanged(TechBuilding* building, uint8 player)
	{
		if( player == GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer() )
		{
			Array<TechBuilding*> buildings;
			_techCtrl->GetNotHiddenBuildings(player, buildings);
			_guiCtrl->AvailableBuildingsChanged(buildings);
		}
	}

	void InteractionController::StartConstructionPlan(TechBuilding* building)
	{
		auto buildMgr = GameWorld::GlobalWorld->GetConstructionManager();
		buildMgr->ChangeConstructionPlanner(building->UsedPlanner);
		buildMgr->StartNewConstructionPlan(
			building->Building, InputManager::Instance()->GetInputState()->GetMouseTerrainPos());
	}

	void InteractionController::AbortConstructionPlan()
	{
		auto buildMgr = GameWorld::GlobalWorld->GetConstructionManager();
		buildMgr->AbandonPlan();
		ChangeStateToIdle();
	}

	void InteractionController::AcceptConstructionPlan()
	{
		auto buildMgr = GameWorld::GlobalWorld->GetConstructionManager();
		auto& res = buildMgr->GetLastPlanningResult();
		if( res.IsLocationValid == false )
		{
			_guiCtrl->ShowQuickNotice(NoticeMessage(
				Notices::GameError, "Invalid location",
				GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer()));
		}
		else
		{
			// Check if player have sufficient resources
			uint8 lackingResource = 0;
			auto& cost = res.UnitDef->GetStats()->Cost;
			auto resources = GameWorld::GlobalWorld->GetResources();
			int client = GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer();
			bool haveResources = resources->CheckPlayerHaveSufficientResources(
				client, cost, &lackingResource);

			if( haveResources == false )
			{
				_guiCtrl->ShowQuickNotice(NoticeMessage(
					Notices::GameError, (string("Insuficient resource: ") + resources->GetResourceName(lackingResource)).c_str(),
					GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer()));
				return;
			}
			/*else
			{
			resources->TakeResourceFromPlayer(client, resNum, cost[i].ResAmount);
			}*/

			buildMgr->AcceptPlan();
			buildMgr->BeginConstruction(res.UnitDef, res.PlannedPosition, 
				GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer());

			if(InputManager::Instance()->CheckModifierIsDown(ModifierCodes::Shift))
				buildMgr->StartNewConstructionPlan(res.UnitDef, res.PlannedPosition);
			else
				ChangeStateToIdle();
		}
	}


	void InteractionController::UpdateConstructionPlanPosition(const Vector3& pos)
	{
		auto buildMgr = GameWorld::GlobalWorld->GetConstructionManager();
		buildMgr->UpdatePlanPosition(pos);
	}


	ICStateConstructionPlan::ICStateConstructionPlan(InteractionController* ic) :
		InputControllerState(ConstructionPlan, ic)
	{ }

	void ICStateConstructionPlan::Update()
	{
		// Hotkeys aborts plan
		if(_inputController->UpdateContextCommands())
		{
			_inputController->AbortConstructionPlan();
			return;
		}

		if(!_inputController->GetGuiController()->IsCursorOverGui())
		{
			if(InputManager::Instance()->CheckMouseWasPressed(MouseButtons::Right))
			{
				_inputController->AbortConstructionPlan();
			}
			// Left click accepts plan	
			else if(InputManager::Instance()->CheckMouseWasPressed(MouseButtons::Left))
			{
				_inputController->AcceptConstructionPlan();
			}
			// If mouse moved, update plan
			else if(InputManager::Instance()->CheckMouseMovedSinceLastFrame())
			{
				_inputController->UpdateConstructionPlanPosition(
					InputManager::Instance()->GetInputState()->GetMouseTerrainPos());
			}
		}
	}

#pragma endregion

#pragma region Minimap

	void InteractionController::OnMinimapLeftDragged(Vector2 minimapPos)
	{
		OnMinimapRightDragged(minimapPos);
	}

	void InteractionController::OnMinimapLeftClicked(Vector2 minimapPos)
	{
		OnMinimapRightClicked(minimapPos);
		_wasMinimapRightClicked = false;
		_wasMinimapLeftClicked = true;
	}

	void InteractionController::OnMinimapRightDragged(Vector2 minimapPos)
	{
		// Move camera so that it center is above given point
		Vector2 dragPos = GameWorld::GlobalWorld->GetMinimap()->
			ConvertMinimapToWorldCoords(minimapPos);
		const Vector3& camPos(_camCtrl->GetCameraPosition());
		float camHeight = camPos.y - GameWorld::GlobalWorld->GetMap()->GetTerrainHeight(camPos.x, camPos.z);
		_camCtrl->SetCameraPosition(Vector3(dragPos.x, 
			GameWorld::GlobalWorld->GetMap()->GetTerrainHeight(dragPos.x, dragPos.y) + camHeight, 
			dragPos.y));
	}

	void InteractionController::OnMinimapRightClicked(Vector2 minimapPos)
	{
		_wasMinimapRightClicked = true;
		_wasMinimapLeftClicked = false;
		Vector2 clickPos = GameWorld::GlobalWorld->GetMinimap()->
			ConvertMinimapToWorldCoords(minimapPos);
		_minimapClickPos = Vector3(clickPos.x, 
			GameWorld::GlobalWorld->GetMap()->GetTerrainHeight(clickPos.x, clickPos.y), 
			clickPos.y);
		_minimapClickUnit = static_cast<Unit*>( // Only units should be minimap selectable
			GameWorld::GlobalWorld->GetMinimap()->HitTest(minimapPos));
		
		_target->SetPointTarget(_minimapClickPos);
		_target->SetTargetUnit(_minimapClickUnit);
	}

#pragma endregion

	ICStateOff::ICStateOff(InteractionController* ic) :
		InputControllerState(Off, ic)
	{ }

	ICStateSelectionIdle::ICStateSelectionIdle(InteractionController* ic) :
		InputControllerState(SelectionIdle, ic)
	{ }

	void ICStateSelectionIdle::Update()
	{
		// Hotkeys have highest priority
		if(_inputController->UpdateContextCommands())
			return;

		if( _inputController->WasMinimapRightClicked() )
		{
			// Get preffered default command for selected units
			_inputController->GetHoverController()->GetCurrentHoverContext()->
				UpdateHoverTarget(_inputController->GetCommandTarget());
			if( _inputController->GetCommandTarget()->GetPrefferedCommand() != OrderTypes::NoCommand )
			{
				_inputController->GetSelectionController()->OrderSelected(OrderTypes::DefaultRight, 
					_inputController->GetCommandTarget(), 
					InputManager::Instance()->CheckModifierIsDown(ModifierCodes::Shift));
			}
		}
		else if( _inputController->WasMinimapLeftClicked() )
		{
			// Select new unit if hovered or deselect old ones
			_inputController->SelectHoveredUnit();
		}
		else if(!_inputController->GetGuiController()->IsCursorOverGui())
		{
			// Right mouse button have higher priority than left
			if(InputManager::Instance()->CheckMouseWasPressed(MouseButtons::Right) &&
				_inputController->GetCommandTarget()->GetPrefferedCommand() != OrderTypes::NoCommand)
			{
				// Get type of command to order and order it to all
				// selected units ( queue if shift is down )
				_inputController->GetSelectionController()->OrderSelected(OrderTypes::DefaultRight, 
					_inputController->GetCommandTarget(), 
					InputManager::Instance()->CheckModifierIsDown(ModifierCodes::Shift));
			}
			else if(InputManager::Instance()->CheckMouseWasPressed(MouseButtons::Left))
			{
				// on left pressed start selection
				_inputController->StartSelection();
			}
		}
	}

	ICStateSelectionSelecting::ICStateSelectionSelecting(InteractionController* ic) :
		InputControllerState(SelectionSelecting, ic),
		_mouseMoved(false)
	{ }

	void ICStateSelectionSelecting::Update()
	{
		// Hotkeys have highest priority
		if(_inputController->UpdateContextCommands())
		{
			_inputController->AbortSelection();
		}
		else if(InputManager::Instance()->CheckMouseWasPressed(MouseButtons::Right))
		{
			// Abort selection on right click
			_inputController->AbortSelection();
		}
		else if(InputManager::Instance()->CheckMouseWasReleased(MouseButtons::Left))
		{
			// Check if mouse is released without moving mouse ->
			// no selection box is created then and only hovered unit selected
			// THis way not owned units can be selected
			// Otherwise end box selection 
			if(_mouseMoved)
			{
				_inputController->EndSelection();
				_mouseMoved = false; // Reset mouse move for next selection
			}
			else
			{
				if(!_inputController->GetGuiController()->IsCursorOverGui())
				{
					_inputController->SelectHoveredUnit();
				}
			}
		}
		else if(InputManager::Instance()->CheckMouseMovedSinceLastFrame())
		{
			if(!_inputController->GetGuiController()->IsCursorOverGui())
			{
				// on mouse move update selection box
				_inputController->UpdateSelection();
				_mouseMoved = true;
			}
		}
	}

	ICStateNoSelectionIdle::ICStateNoSelectionIdle(InteractionController* ic) :
		InputControllerState(NoSelectionIdle, ic)
	{ }

	void ICStateNoSelectionIdle::Update()
	{
		if( _inputController->WasMinimapLeftClicked() )
		{
			// Select new unit if hovered
			_inputController->SelectHoveredUnit();
		}
		else if(!_inputController->GetGuiController()->IsCursorOverGui())
		{
			if(InputManager::Instance()->CheckMouseWasPressed(MouseButtons::Left))
			{
				// on left pressed start selection
				_inputController->StartSelection();
			}
		}
	}

	ICStateNoSelectionSelecting::ICStateNoSelectionSelecting(InteractionController* ic) :
		InputControllerState(NoSelectionSelecting, ic),
		_mouseMoved(false)
	{ }

	void ICStateNoSelectionSelecting::Update()
	{

		if(InputManager::Instance()->CheckMouseWasPressed(MouseButtons::Right))
		{
			_inputController->AbortSelection();
		}
		else if(InputManager::Instance()->CheckMouseWasReleased(MouseButtons::Left))
		{
			// Check if mouse is released without moving mouse ->
			// no selection box is created then and only hovered unit selected
			// THis way not owned units can be selected
			// Otherwise end box selection 
			if(_mouseMoved)
			{
				_inputController->EndSelection();
				_mouseMoved = false; // Reset mouse move for next selection
			}
			else
			{
				if(!_inputController->GetGuiController()->IsCursorOverGui())
				{
					_inputController->SelectHoveredUnit();
				}
			}
		}
		else if(InputManager::Instance()->CheckMouseMovedSinceLastFrame())
		{
			if(!_inputController->GetGuiController()->IsCursorOverGui())
			{
				// on mouse move update selection box
				_inputController->UpdateSelection();
				_mouseMoved = true;
			}
		}
	}
}