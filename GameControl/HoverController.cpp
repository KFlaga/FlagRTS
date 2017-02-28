#include "HoverController.h"
#include <InputManager.h>
#include <GameWorld.h>
#include <Map.h>
#include <PhysicalObject.h>
#include <MainGameObjectPool.h>
#include <MapDecorator.h>
#include <Unit.h>
#include <HoverContext.h>
#include <Command.h>
#include <HoverTargetTypes.h>
#include "DefaultHoverContextes.h"
#include <CursorManager.h>
#include <SelectionFlags.h>
#include <BillboardMarker.h>
#include <ModelMarker.h>
#include <Exception.h>

namespace FlagRTS
{
	HoverController::HoverController(Ogre::SceneManager* ogreMgr) :
		_ogreMgr(ogreMgr),
		_hoveredObject(0), 
		_noSelectionContext(xNew0(EmptySelectionHoverContext)),
		_selectionContext(xNew0(UnitGroupHoverContext)),
		_disabledContext(xNew0(DisabledHoverContext)),
		_onHoverTargetDespawned(this)
	{
		_commandContextes.insert(std::make_pair(OrderTypes::Move,
			xNew0(MoveCommandHoverContext)));
		_commandContextes.insert(std::make_pair(OrderTypes::Attack,
			xNew0(AttackCommandHoverContext)));
	}

	HoverController::~HoverController()
	{
		for(auto hcIt = _commandContextes.begin(), end = _commandContextes.end(); hcIt != end; ++hcIt)
		{
			xDelete(hcIt->second);
		}
		xDelete(_noSelectionContext);
		xDelete(_selectionContext);
		xDelete(_disabledContext);
	}

	void HoverController::Update(const Ogre::Ray& cameraRay, CommandTarget* target)
	{
		// First update cursor position
		InputState* is = InputManager::Instance()->GetInputState();
		is->UpdateMouseRealPos(cameraRay.getOrigin());
		target->SetTargetUnit(0);

		// get terrain grid and check intersection
		is->UpdateMouseTerrainPos(GameWorld::GlobalWorld->GetMap()->GetTerrainInterscetion(cameraRay));
		if(is->GetMouseTerrainPos().x < 0)
		{
			target->SetTargetType(HoverTargetTypes::Invalid);
		}

		SceneObject* newHovered = CheckHover(cameraRay);

		// Check if hover changed, and if yes fire/add events
		if( newHovered != _hoveredObject )
		{
			if(_hoveredObject != 0)
			{
				_hoveredObject->MouseHoverEnd();
				_hoveredObject->Despawned() -= &_onHoverTargetDespawned;
			}
			if(newHovered != 0)
			{
				newHovered->MouseHoverBegin();
				newHovered->Despawned() += &_onHoverTargetDespawned;
			}
		}
		_hoveredObject = newHovered;

		Unit* hoveredUnit = _hoveredObject != 0 && _hoveredObject->GetFinalType() == GetTypeId<Unit>() ?
			static_cast<Unit*>(_hoveredObject) : 0;
		target->SetTargetUnit(hoveredUnit);
		if(hoveredUnit != 0)
		{
			target->SetPointTarget(	hoveredUnit->GetPositionAbsolute());
		}
		else
		{
			target->SetPointTarget(InputManager::Instance()->GetMouseTerrainPos());
		}

		_currentContext->UpdateHoverTarget(target);
		string cursor = _currentContext->GetCursor();
		// Set cursor if cursor changed
		if(cursor.compare(_lastCursor) != 0)
		{
			CursorManager::Instance()->SetCurrentCursorState(cursor);
			_lastCursor = cursor;
		}
	}

	SceneObject* HoverController::CheckHover(const Ogre::Ray& cameraRay)
	{
		float minDist = 99999.0f;
		auto rayQuery = _ogreMgr->createRayQuery(cameraRay, SelectionFlags::Hoverable);
		auto& queryResult = rayQuery->execute();
		SceneObject* newHovered = 0;

		// Find nearest Unit or SceneMarker -> interactable SMs have higher hover priority
		// ( they should be used as object-guis )
		for(auto objIt = queryResult.begin(); objIt != queryResult.end(); ++objIt)
		{
			auto sceneObj = objIt->movable->getUserAny().get<SceneObject*>();
			if(sceneObj->GetFinalType() == GetTypeId<BillboardMarker>() || 
				sceneObj->GetFinalType() == GetTypeId<ModelMarker>())
			{
				if(objIt->distance < minDist)
				{
					newHovered = sceneObj;
					minDist = objIt->distance;
				}
			}
		}

		// No marker was hovered -> check units
		if( newHovered == 0 )
		{
			minDist = 99999.f;
			for(auto objIt = queryResult.begin(); objIt != queryResult.end(); ++objIt)
			{
				auto sceneObj = objIt->movable->getUserAny().get<SceneObject*>();
				if(sceneObj->GetFinalType() == GetTypeId<Unit>())
				{
					if(objIt->distance < minDist)
					{
						newHovered = sceneObj;
						minDist = objIt->distance;
					}
				}
			}
		}

		return newHovered;
	}

	void HoverController::ChangeHoverContextToIdle()
	{
		_currentContext = _noSelectionContext;
		_hoveredObject = 0;
		_lastCursor = "";
	}

	void HoverController::ChangeHoverContextToUnitGroup(UnitClass groupClass)
	{
		_currentContext = _selectionContext;
		_currentContext->SetGroupClass(groupClass);
		_hoveredObject = 0;
		_lastCursor = "";
	}

	void HoverController::ChangeHoverContextToCommand(Command* command, UnitClass groupClass)
	{
		// Hover context is choosen based on:
		// -- command order type -> for default commands
		// -- command cast type
		// -- command aligment type
		// summed up to command full type for abilities
		if(command->GetOrderType() == OrderTypes::Ability)
		{

		}
		else
		{
			auto contextIt = _commandContextes.find(command->GetOrderType());
			if(contextIt != _commandContextes.end())
			{
				_currentContext = contextIt->second;
				_currentContext->SetGroupClass(groupClass);
				_hoveredObject = 0;
				_lastCursor = "";
			}
			else
			{
				CastException_d(string("No HoverContext for command : ") + std::to_string(command->GetOrderType()));
				// Set some default context
			}
		}
	}

	void HoverController::ChangeHoverContextToHold()
	{
		_currentContext = _disabledContext;
		_lastCursor = "";
	}

	Unit* HoverController::GetHoveredUnit() const 
	{ 
		return dynamic_cast<Unit*>(_hoveredObject);
	}
}