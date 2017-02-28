#include "SelectionController.h"
#include "CameraController.h"
#include <Command.h>
#include <CommandTarget.h>
#include <OgreSceneManager.h>
#include <SelectionBox.h>
#include <SceneObjectProperties.h>
#include <SelectionFlags.h>
#include <Player.h>
#include <GameWorld.h>
#include <Unit.h>
#include <SelectionComponent.h>

#include <AudioManager.h>
#include <SoundChannel.h>
#include <UnitSoundSet.h>

namespace FlagRTS
{
	class SceneQuery_Select : public Ogre::SceneQueryListener
	{
	protected:
		SelectionController* _selector;

	public:
		SceneQuery_Select(SelectionController* selector) :
			_selector(selector)
		{ }

		bool queryResult(Ogre::MovableObject* object) 
		{
			_selector->SelectSilient(object->getUserObjectBindings().getUserAny().get<Unit*>());
			return true;
		}
		bool queryResult(Ogre::SceneQuery::WorldFragment* wf) { return true; }
	};

	class SceneQuery_Deselect : public Ogre::SceneQueryListener
	{
	protected:
		SelectionController* _selector;

	public:
		SceneQuery_Deselect(SelectionController* selector) :
			_selector(selector)
		{ }

		bool queryResult(Ogre::MovableObject* object) 
		{
			_selector->DeselectSilient(object->getUserObjectBindings().getUserAny().get<Unit*>());
			return true;
		}
		bool queryResult(Ogre::SceneQuery::WorldFragment* wf) { return true; }
	};

	class SceneQuery_AddToSelection : public Ogre::SceneQueryListener
	{
	protected:
		SelectionController* _selector;
		uint32 _desiredFlags;
		uint32 _undesiredFlags;

	public:
		SceneQuery_AddToSelection(SelectionController* selector) :
			_selector(selector)
		{ }

		bool queryResult(Ogre::MovableObject* object) 
		{
			// Add only owned, selectable units with any flag from desired set and none from undesired set
			SceneObject* obj = object->getUserObjectBindings().getUserAny().get<SceneObject*>();

			// Check if it have selection component
			SelectionComponent* comp = obj->FindComponent<SelectionComponent>();

			if( comp != 0 &&
				obj->GetFinalType() == GetTypeId<Unit>() &&
				comp->IsSelectable() // &&
				//(pobj->GetSelectionFlags() & _desiredFlags) > 0 &&
				//(pobj->GetSelectionFlags() & _undesiredFlags) == 0
				)
			{
				Unit* unit = static_cast<Unit*>(obj);
				if(unit->GetOwner() == GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer())
				{
					_selector->AddToSelectionSilient(unit);
				}
			}
			return true;
		}
		bool queryResult(Ogre::SceneQuery::WorldFragment* wf) { return true; }

		void SetDesiredFlags(uint32 flags = (uint32)-1) { _desiredFlags = flags; }
		void SetUndesiredFlags(uint32 flags = 0u) { _undesiredFlags = flags; }
	};

	SelectionController::SelectionController(CameraController* camCtrl, Ogre::SceneManager* ogre) :
		_ogreMgr(ogre),
		_camController(camCtrl),
		_selectedUnits(xNew0(UnitGroup)),
		_sbox(xNew0(SelectionBox)),
		_querySelect(xNew1(SceneQuery_Select, this)),
		_queryAddToSelection(xNew1(SceneQuery_AddToSelection, this)),
		_queryDeselect(xNew1(SceneQuery_Deselect, this)),
		_currentSubgroup(0),
		_currentSubgroupType(0),
		_isNotOwnedUnitSelected(false),
		_nextSelectionSound(0),
		_onSelectedUnitDied(this)
	{ 
		// Lambda function to change camera used by SelectionBox when active camera changes
		auto changeCamera = [this](GameCamera* camera) 
		{
			_sbox->AbortSelection();
			_sbox->ChangeActiveCamera(camera);
		};

		typedef DelegateEventHandler<decltype(changeCamera), GameCamera*> CameraDelegate;
		_camController->ActiveCameraChanged() += xNew1(CameraDelegate,changeCamera);

		if(_camController->GetActiveCamera() != 0)
		{
			_sbox->ChangeActiveCamera(_camController->GetActiveCamera());
		}

		_selectedUnits->UnitFromGroupDied() += &_onSelectedUnitDied;
	}

	SelectionController::~SelectionController()
	{
		xDelete(_querySelect);
		xDelete(_queryAddToSelection);
		xDelete(_queryDeselect);
		xDelete(_sbox);
		xDeleteSafe(_selectedUnits);
	}

	void SelectionController::Update(float ms)
	{

	}

	void SelectionController::OrderSelected(Command* command, CommandTarget* target, bool queue)
	{
		// TODO take into account appliance flags
		OrderSelected(command->GetOrderType(), target, queue);
	}

	void SelectionController::StartSelection(const Vector2& mousePos)
	{
		_sbox->StartSelection(mousePos);
	}

	void SelectionController::UpdateSelection(const Vector2& mousePos)
	{
		_sbox->UpdateSelection(mousePos);
	}

	void SelectionController::EndSelection(bool deselectOld)
	{
		if(deselectOld)
			DeselectAllSilient();
		_sbox->EndSelection();
		AddAllFromBox(_sbox);
	}

	void SelectionController::AbortSelection()
	{
		_sbox->AbortSelection();
	}

	void SelectionController::SelectHovered(CommandTarget* hoverTarget, bool deselectOld)
	{
		if(deselectOld)
			DeselectAllSilient();
		if(hoverTarget->GetTargetUnit() != 0 )
		{
			// If selected group count > 0, then select only owned unit
			if(GameWorld::GlobalWorld->GetPlayers()->IsOwned(hoverTarget->GetTargetUnit()->GetOwner()) )
			{
				AddToSelectionSilient(hoverTarget->GetTargetUnit());
				PlaySelectionSound(hoverTarget->GetTargetUnit());
			}
			else if(_selectedUnits->GroupCount() != 0)
			{
				AddToSelectionSilient(hoverTarget->GetTargetUnit());
			}
		}
		_selectionChanged.Fire(_selectedUnits);
		UpdateContextSubgroup();
	}

	void SelectionController::SelectAllFromBox(SelectionBox* sbox)
	{
		DeselectAllSilient();

		try
		{
			auto sceneQuery = CreateSceneQuery(sbox);

			sceneQuery->execute(_queryAddToSelection);
			_ogreMgr->destroyQuery(sceneQuery);
		}
		catch(...) 
		{ }
		_selectionChanged.Fire(_selectedUnits);
		UpdateContextSubgroup();

		// Play sound for units from context subgroup
		if( _currentSubgroup != 0 )
		{
			PlaySelectionSound(_currentSubgroup->front());
		}
	}

	void SelectionController::AddAllFromBox(SelectionBox* sbox)
	{
		try
		{
			auto sceneQuery = CreateSceneQuery(sbox);

			sceneQuery->execute(_queryAddToSelection);
			_ogreMgr->destroyQuery(sceneQuery);
		}
		catch(...) 
		{ }

		_selectionChanged.Fire(_selectedUnits);
		UpdateContextSubgroup();

		// Play sound for units from context subgroup
		if( _currentSubgroup != 0 )
		{
			PlaySelectionSound(_currentSubgroup->front());
		}
	}

	void SelectionController::SelectUnit(Unit* unit)
	{
		_ASSERT(unit != 0);
		DeselectAllSilient();
		AddToSelectionSilient(unit);
		_selectionChanged.Fire(_selectedUnits);
		UpdateContextSubgroup();

		// This function is called when selecting one unit - play selection siund for it
		// if unit is owned
		if(unit->GetOwner() == GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer())
		{
			PlaySelectionSound(unit);
		}
	}

	void SelectionController::SelectSilient(Unit* unit)
	{
		_ASSERT(unit != 0);
		DeselectAllSilient();
		AddToSelection(unit);
	}

	void SelectionController::AddToSelection(Unit* unit)
	{
		_ASSERT(unit != 0);
		_ASSERT(unit->FindComponent<SelectionComponent>() != 0);
		SelectionComponent* comp = unit->FindComponent<SelectionComponent>();
		comp->SetIsSelected(true);
		_selectedUnits->AddToGroup(unit);
		_selectionChanged.Fire(_selectedUnits);
		UpdateContextSubgroup();
	}

	void SelectionController::AddToSelectionSilient(Unit* unit)
	{
		_ASSERT(unit != 0);
		_ASSERT(unit->FindComponent<SelectionComponent>() != 0);
		SelectionComponent* comp = unit->FindComponent<SelectionComponent>();
		comp->SetIsSelected(true);
		_selectedUnits->AddToGroup(unit);
	}

	void SelectionController::DeselectUnit(Unit* unit)
	{
		_ASSERT(unit != 0);
		_ASSERT(unit->FindComponent<SelectionComponent>() != 0);
		SelectionComponent* comp = unit->FindComponent<SelectionComponent>();
		comp->SetIsSelected(true);
		_selectedUnits->RemoveFromGroup(unit);
		_selectionChanged.Fire(_selectedUnits);
		UpdateContextSubgroup();
	}

	void SelectionController::DeselectSilient(Unit* unit)
	{
		_ASSERT(unit != 0);
		_ASSERT(unit->FindComponent<SelectionComponent>() != 0);
		SelectionComponent* comp = unit->FindComponent<SelectionComponent>();
		comp->SetIsSelected(true);
		_selectedUnits->RemoveFromGroup(unit);
	}

	void SelectionController::DeselectAll()
	{
		//_currentSubgroupType = 0;
		_selectedUnits->ForEach( [](Unit* unit)
		{ 
			SelectionComponent* comp = unit->FindComponent<SelectionComponent>();
			comp->SetIsSelected(false); 
		} );
		_selectedUnits->RemoveAll();
		_selectionChanged.Fire(_selectedUnits);
		UpdateContextSubgroup();
	}

	void SelectionController::DeselectAllSilient()
	{
		//_currentSubgroupType = 0;
		_selectedUnits->ForEach( [](Unit* unit)
		{ 
			SelectionComponent* comp = unit->FindComponent<SelectionComponent>();
			comp->SetIsSelected(false); 
		} );
		_selectedUnits->RemoveAll();
	}

	void SelectionController::SelectAllInRange()
	{

	}

	void SelectionController::SelectAllInRangeOfType(size_t type)
	{

	}

	void SelectionController::RemoveUnitTypeFromSelection(size_t type)
	{
		auto units = _selectedUnits->GetUnitsOfType(type);
		_ASSERT( units != 0 );
		if(units == 0)
			return;

		for(auto unitIt = units->begin(); unitIt != units->end(); ++unitIt)
		{
			SelectionComponent* comp = (*unitIt)->FindComponent<SelectionComponent>();
			comp->SetIsSelected(false); 
		}

		_selectedUnits->RemoveAllOfType(type);
		_selectionChanged.Fire(_selectedUnits);
		UpdateContextSubgroup();
	}

	void SelectionController::LeaveOnlyUnitTypeInSelection(size_t type)
	{
		auto unitTypeIt = _selectedUnits->GetUnitGroupBegin();
		while(unitTypeIt != _selectedUnits->GetUnitGroupEnd())
		{
			if(unitTypeIt->first == type)
				++unitTypeIt;
			else
			{
				for(auto unitIt = unitTypeIt->second.begin(); unitIt != unitTypeIt->second.end(); ++unitIt)
				{
					SelectionComponent* comp = (*unitIt)->FindComponent<SelectionComponent>();
					comp->SetIsSelected(false); 
				}
				unitTypeIt = _selectedUnits->GetUnitsMap()->erase(unitTypeIt);
			}
		}
		_selectionChanged.Fire(_selectedUnits);
		UpdateContextSubgroup();
	}

	Ogre::RegionSceneQuery* SelectionController::CreateSceneQuery(SelectionBox* sbox)
	{
		// Select only selectable
		uint32 selectionFlags = SelectionFlags::Selectable;

		return _ogreMgr->createAABBQuery( Ogre::AxisAlignedBox(
			Vector3(std::min(sbox->GetTopLeft().x, sbox->GetBotRight().x), 
			-200.0f, std::min(sbox->GetTopLeft().z, sbox->GetBotRight().z)),
			Vector3(std::max(sbox->GetTopLeft().x, sbox->GetBotRight().x), 
			1000.0f, std::max(sbox->GetTopLeft().z, sbox->GetBotRight().z))),
			selectionFlags);
	}

	void SelectionController::SetContextSubgroupToType(size_t type)
	{
		auto subgroupIt = _selectedUnits->GetUnitsOfTypeIterator(type);
		if(subgroupIt != _selectedUnits->GetUnitGroupEnd())
			_currentSubgroup = &subgroupIt->second;
		else
			_currentSubgroup = 0;
		_subgroupChanged.Fire(_currentSubgroup);
	}

	void SelectionController::UpdateContextSubgroup()
	{
		//bool wasNotOwnedSelected = _isNotOwnedUnitSelected;
		_isNotOwnedUnitSelected = _selectedUnits->GroupCount() == 1 &&
			!GameWorld::GlobalWorld->GetPlayers()->IsOwned((*_selectedUnits->GetUnitGroupBegin()->second.begin())->GetOwner());

		if( _isNotOwnedUnitSelected ||
			_selectedUnits->GroupCount() == 0)
		{
			// If not-owned unit is selected or no unit is selected, then there's no 
			// context subgroup
			_currentSubgroupType = 0;
			_currentSubgroup = 0;
			_subgroupChanged.Fire(0);
			return;
		}

		_currentSubgroupIt = _selectedUnits->GetUnitsOfTypeIterator(_currentSubgroupType);
		if(_currentSubgroupIt == _selectedUnits->GetUnitGroupEnd() )
		{
			// Last group was removed/not set, so set the first one
			_currentSubgroupIt = _selectedUnits->GetUnitGroupBegin();
		}
		// else
		// Old subgroup is still active, so just dont change it
		_currentSubgroup = &_currentSubgroupIt->second;
		_currentSubgroupType = _currentSubgroupIt->first;
		_subgroupChanged.Fire(_currentSubgroup);
	}

	void SelectionController::NextContextSubgroup()
	{
		if(_selectedUnits->GroupCount() == 0)
			return;

		if((++_currentSubgroupIt) == _selectedUnits->GetUnitGroupEnd())
		{
			_currentSubgroupIt = _selectedUnits->GetUnitGroupBegin();
		}

		_currentSubgroup = &_currentSubgroupIt->second;
		_currentSubgroupType = _currentSubgroupIt->first;
		_subgroupChanged.Fire(_currentSubgroup);
	}

	void SelectionController::PlaySelectionSound(Unit* unit)
	{
		if( _nextSelectionSound >= 3 )
			_nextSelectionSound = 0;

		Media::SoundSource* sound = unit->GetUnitDefinition()->GetUnitSoundSet()
			[UnitSounds::OnSelected1 + _nextSelectionSound];

		auto ch1 = Media::AudioManager::Instance()->GetChannel( Media::SoundChannel::CH_SelectionAcknowledge1 );
		ch1->Play( sound, true );

		++_nextSelectionSound;
	}

	void SelectionController::SelectedUnitDied(UnitGroup* group, Unit* unit)
	{
		_ASSERT(unit != 0);
		_ASSERT(unit->FindComponent<SelectionComponent>() != 0);
		SelectionComponent* comp = unit->FindComponent<SelectionComponent>();
		comp->SetIsSelected(false); 
		_selectionChanged.Fire(_selectedUnits);
		UpdateContextSubgroup();
	}
}