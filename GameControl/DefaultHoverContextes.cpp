#include "DefaultHoverContextes.h"
#include <Unit.h>
#include <MapDecorator.h>
#include <Player.h>
#include <HoverTargetTypes.h>
#include <UnitClass.h>

#include <InputManager.h>
#include <GameWorld.h>
#include <PathingSystem.h>
#include <PathFindingUniformGridPathingMap.h>

namespace FlagRTS
{
	DisabledHoverContext::DisabledHoverContext() :
		HoverContext()
	{

	}

	void DisabledHoverContext::UpdateHoverTarget(CommandTarget* hoverTarget)
	{
		_cursorName = "Default";
		hoverTarget->SetTargetType(HoverTargetTypes::NoTarget);
	}

	UnitGroupHoverContext::UnitGroupHoverContext() :
		HoverContext()
	{
		_groupClass = UnitClasses::None;
	}

	void ProcessTerrainContext_UnitGroup(UnitClass uclass, int& htype, string& cursorName, int& commandType);
	void ProcessOwnUnitsContext_UnitGroup(UnitClass uclass, Unit* unit, string& cursorName, int& commandType);
	void ProcessEnemyUnitsContext_UnitGroup(UnitClass uclass, Unit* unit, string& cursorName, int& commandType);

	void UnitGroupHoverContext::UpdateHoverTarget(CommandTarget* hoverTarget)
	{
		Unit* hoveredUnit = hoverTarget->GetTargetUnit();

		_cursorName = "Default";
		int htype = HoverTargetTypes::NoTarget;
		int command = OrderTypes::NoCommand;
		auto pathMap = 	GameWorld::GlobalWorld->GetPathingSystem()->GetPathingMap();

		// 1) Cursor not over game map
		if(InputManager::Instance()->GetMouseTerrainPos().x < 0.0f)
		{
			_cursorName = "Invalid";
			hoverTarget->SetTargetType(HoverTargetTypes::Invalid);
		}
		// 2) Cursor over non-unit
		else if(hoveredUnit == 0)
		{
			_cursorName = "Default";
			htype |= HoverTargetTypes::Terrain;

			// Check pathing map for obstacles
			PathFinding::CollisionFilter cf = pathMap->GetCell(pathMap->FindCellPositionFallsInto(
				Vector2(hoverTarget->GetPointTarget().x, hoverTarget->GetPointTarget().z)));

			htype |= (cf.Blocking & CollisionType::Ground) > 0 ? 
				HoverTargetTypes::ObstacleGround : 0;
			htype |= (cf.Blocking & CollisionType::Air) > 0 ? 
				HoverTargetTypes::ObstacleAir : 0;

			ProcessTerrainContext_UnitGroup(_groupClass, htype, _cursorName, command);
		}
		else 
		{
			int clientPlayer = GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer();
			hoverTarget->SetTargetType(HoverTargetTypes::Unit);
			//	// if(not stealthed / non-interactable / is visible etc)
			// 4.1) Unit is owned
			if(hoveredUnit->GetOwner() == clientPlayer)
			{
				hoverTarget->SetTargetType(HoverTargetTypes::OwnUnit);
				ProcessOwnUnitsContext_UnitGroup(_groupClass, hoveredUnit, _cursorName, command);
			}
			// 4.2) Unit is allied
			else if(GameWorld::GlobalWorld->GetPlayers()->IsAlly(clientPlayer, hoveredUnit->GetOwner()))
			{
				_cursorName = "Follow";
				htype = HoverTargetTypes::AllyUnit;
			}
			// 4.3) Unit is enemy
			else if(GameWorld::GlobalWorld->GetPlayers()->IsEnemy(clientPlayer, hoveredUnit->GetOwner()))
			{
				_cursorName = "Attack";
				htype = HoverTargetTypes::EnemyUnit;
			}
			// 4.4) Unit is neutral
			else if(GameWorld::GlobalWorld->GetPlayers()->IsNeutral(clientPlayer, hoveredUnit->GetOwner()))
			{
				_cursorName = "Neutral";
				htype = HoverTargetTypes::NeutralUnit;
			}
		}

		hoverTarget->SetPrefferedCommand(command);
		hoverTarget->SetTargetType(htype);
	}

	void ProcessTerrainContext_UnitGroup(UnitClass uclass, int& htype, string& cursorName, int& commandType)
	{
		// Some rules for cursors ( with this priority ) EXTRACT FUNCTION
		// - in group there are mobile ground units and ground is passable -> move cursor
		// - in group there are mobile air units and air is passable -> move cursor
		// - else if there are mobile units -> cannot-move cursor
		// - in group is unit factory with ground only units, only buildings are selected and
		// terrain is ground passable -> set rally point cursor
		// - in group is unit factory with air only units, only buildings are selected and
		// terrain is air passable -> set rally point 
		// - in group is unit factory with both type units, only buildings are selected and
		// terrain is passable for at least one kind of them -> set rally point 
		// - else set to default cursor
		if( (UnitClasses::IsUnitOfComplexClass( uclass, UnitClasses::MobileGround ) &&
			(htype & HoverTargetTypes::ObstacleGround) == 0) ||
			(UnitClasses::IsUnitOfComplexClass( uclass, UnitClasses::MobileAir ) &&
			(htype & HoverTargetTypes::ObstacleAir) == 0) )
		{
			cursorName = "Move";
			htype |= HoverTargetTypes::Passable;
			commandType = OrderTypes::Move;
		}
		else if( UnitClasses::IsUnitOfSubClass( uclass, UnitClasses::Mobile ) )
		{
			cursorName = "CannotMove";
			commandType = OrderTypes::NoCommand;
		}
		// Units are only immobile unit factories - for now treat them as buildings
		else
		{
			if( (UnitClasses::IsUnitOfSubClass( uclass, UnitClasses::TrainUnits ) &&
				(htype & HoverTargetTypes::ObstacleGround) == 0) ||
				((UnitClasses::IsUnitOfSubClass( uclass, UnitClasses::TrainUnits ) &&
				(htype & HoverTargetTypes::ObstacleAir) == 0)) )
			{
				cursorName = "RallyPoint";
				commandType = OrderTypes::RallyPoint;
			}
			else if(UnitClasses::IsUnitOfSubClass( uclass, UnitClasses::TrainUnits ) || 
				UnitClasses::IsUnitOfSubClass( uclass, UnitClasses::TrainUnits ) )
			{
				cursorName = "CannotMove";
				commandType = OrderTypes::NoCommand;
			}
			else
			{
				cursorName = "Default";
				commandType = OrderTypes::NoCommand;
			}
		}
	}

	void ProcessOwnUnitsContext_UnitGroup(UnitClass uclass, Unit* unit, string& cursorName, int& commandType)
	{
		// Some rules for cursors/commands :
		// - unit is mine/rafinery and UC is resource transporter -> transport resources
		// - unit is non-finished building, UC > worker -> build
		// - unit is factory, UC > worker -> aid training
		// - unit is other building, UC > worker -> aid-repair
		// Mabye more universal Aid command for worker
		// - unit is mobile and UC is mobile -> follow
		// - unit is injured and UC > healer -> heal
		UnitClass targetClass = unit->GetUnitClass();
		if( UnitClasses::IsUnitOfSubClass(targetClass, UnitClasses::Structure) )
		{
			// Unit is building
			if( UnitClasses::IsUnitOfSubClass(targetClass, UnitClasses::ResourceFactory) &&
				UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::TransportResources) )
			{
				cursorName = "PickUp";
				commandType = OrderTypes::Gather;
			}
			else if( UnitClasses::IsUnitOfSubClass(targetClass, UnitClasses::ResourceStorage) &&
				UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::TransportResources) )
			{
				cursorName = "Return";
				commandType = OrderTypes::ReturnResources;
			}
			// else if( unit->IsFinished() ) ??
			else if( unit->GetHitPoints() < unit->GetMaxHitPoints() &&
				UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::Repairer ) )
			{
				cursorName = "Repair";
				commandType = OrderTypes::Repair;
			}
			else if(  (UnitClasses::IsUnitOfSubClass(targetClass, UnitClasses::TrainUnits) ||
				UnitClasses::IsUnitOfSubClass(targetClass, UnitClasses::TrainUnits)) &&
				UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::Worker) )
			{
				cursorName = "Aid";
				commandType = OrderTypes::Train;
			}
		}
		else if( unit->GetHitPoints() < unit->GetMaxHitPoints() &&
			UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::Healer ) )
		{
			cursorName = "Heal";
			commandType = OrderTypes::Repair;
		}
		else if( UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::Mobile) )
		{
			cursorName = "Move";
			commandType = OrderTypes::Move;
		}
		else
		{
			cursorName = "SelectOwn";
			commandType = OrderTypes::NoCommand;
		}
	}

	void ProcessEnemyUnitsContext_UnitGroup(UnitClass uclass, Unit* unit, string& cursorName, int& commandType)
	{
		// Some rules for cursors/commands :
		// - unit can attack -> attack (all aggresive)
		// - unit can convert -> convert
	}

	EmptySelectionHoverContext::EmptySelectionHoverContext() :
		HoverContext()
	{

	}

	void EmptySelectionHoverContext::UpdateHoverTarget(CommandTarget* hoverTarget)
	{
		Unit* hoveredUnit = hoverTarget->GetTargetUnit();

		//// Check for every possible target type
		_cursorName = "Default";
		hoverTarget->SetTargetType(HoverTargetTypes::NoTarget);

		// 2) Cursor over terrain
		if(hoveredUnit == 0)
		{
			_cursorName = "Default";
			hoverTarget->SetTargetType(HoverTargetTypes::Terrain);
		}
		// 4) Cursor over unit -> cast object to unit
		else
		{
			int clientPlayer = GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer();
			hoverTarget->SetTargetType(HoverTargetTypes::Unit);
			//	// if(not stealthed / non-interactable / is visible etc)
			//	// 4.1) Unit is owned
			if(hoveredUnit->GetOwner() == clientPlayer)
			{
				_cursorName = "SelectOwn";
				hoverTarget->SetTargetType(HoverTargetTypes::OwnUnit);
			}
			// 4.2) Unit is allied
			else if(GameWorld::GlobalWorld->GetPlayers()->IsAlly(clientPlayer, hoveredUnit->GetOwner()))
			{
				_cursorName = "SelectAlly";
				hoverTarget->SetTargetType(HoverTargetTypes::AllyUnit);
			}
			// 4.3) Unit is enemy
			else if(GameWorld::GlobalWorld->GetPlayers()->IsEnemy(clientPlayer, hoveredUnit->GetOwner()))
			{
				_cursorName = "SelectEnemy";
				hoverTarget->SetTargetType(HoverTargetTypes::EnemyUnit);
			}
			// 4.4) Unit is neutral
			else if(GameWorld::GlobalWorld->GetPlayers()->IsNeutral(clientPlayer, hoveredUnit->GetOwner()))
			{
				_cursorName = "SelectNeutral";
				hoverTarget->SetTargetType(HoverTargetTypes::NeutralUnit);
			}
		}
	}


	MoveCommandHoverContext::MoveCommandHoverContext()
	{
		_groupClass = UnitClasses::None;
	}

	void MoveCommandHoverContext::UpdateHoverTarget(CommandTarget* hoverTarget)
	{
		Unit* hoveredUnit = hoverTarget->GetTargetUnit();

		//// Check for every possible target type
		_cursorName = "TargetDefault";
		hoverTarget->SetTargetType(HoverTargetTypes::NoTarget);

		// 1) Cursor not over game map
		if(InputManager::Instance()->GetMouseTerrainPos().x < 0.0f)
		{
			_cursorName = "Invalid";
			hoverTarget->SetTargetType(HoverTargetTypes::Invalid);
		}

		// 2) Cursor over terrain ( no object )
		else if(hoveredUnit == 0)
		{
			// TODO : Cursor depends on unit class
			_cursorName = "TargetNeutral";
			hoverTarget->SetTargetType(HoverTargetTypes::Passable);
			// 2.1) Terrain is ground-passable ( flat terrain ), but not naval-passable

			// 2.2) Terrain is naval-passable ( water ), but not ground-passable

			// 2.3) Terrain is only air-passable ( cliff ) -> may add cliffs as objects, then move it from there
		}
		else 
		{
			int clientPlayer = GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer();
			hoverTarget->SetTargetType(HoverTargetTypes::Unit);
			//	// if(not stealthed / non-interactable / is visible etc)
			// 4.1) Unit is owned
			if(hoveredUnit->GetOwner() == clientPlayer)
			{
				_cursorName = "TargetFriendly";
				hoverTarget->SetTargetType(HoverTargetTypes::OwnUnit);
			}
			// 4.2) Unit is allied
			else if(GameWorld::GlobalWorld->GetPlayers()->IsAlly(clientPlayer, hoveredUnit->GetOwner()))
			{
				_cursorName = "TargetFriendly";
				hoverTarget->SetTargetType(HoverTargetTypes::AllyUnit);
			}
			// 4.3) Unit is enemy
			else if(GameWorld::GlobalWorld->GetPlayers()->IsEnemy(clientPlayer, hoveredUnit->GetOwner()))
			{
				_cursorName = "TargetHostile";
				hoverTarget->SetTargetType(HoverTargetTypes::EnemyUnit);
			}
			// 4.4) Unit is neutral
			else if(GameWorld::GlobalWorld->GetPlayers()->IsNeutral(clientPlayer, hoveredUnit->GetOwner()))
			{
				_cursorName = "TargetNeutral";
				hoverTarget->SetTargetType(HoverTargetTypes::NeutralUnit);
			}
		}
	}

	AttackCommandHoverContext::AttackCommandHoverContext()
	{
		_groupClass = UnitClasses::None;
	}

	void AttackCommandHoverContext::UpdateHoverTarget(CommandTarget* hoverTarget)
	{
		Unit* hoveredUnit = hoverTarget->GetTargetUnit();

		//// Check for every possible target type
		_cursorName = "TargetHostile";
		hoverTarget->SetTargetType(HoverTargetTypes::NoTarget);

		// 1) Cursor not over game map
		if(InputManager::Instance()->GetMouseTerrainPos().x < 0.0f)
		{
			_cursorName = "Invalid";
			hoverTarget->SetTargetType(HoverTargetTypes::Invalid);
		}

		// 2) Cursor over terrain ( no object )
		else if(hoveredUnit == 0)
		{
			// TODO : Cursor depends on unit class
			_cursorName = "TargetNeutral";
			hoverTarget->SetTargetType(HoverTargetTypes::Passable);
			// 2.1) Terrain is ground-passable ( flat terrain ), but not naval-passable

			// 2.2) Terrain is naval-passable ( water ), but not ground-passable

			// 2.3) Terrain is only air-passable ( cliff ) -> may add cliffs as objects, then move it from there
		}
		else 
		{
			int clientPlayer = GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer();
			hoverTarget->SetTargetType(HoverTargetTypes::Unit);
			//	// if(not stealthed / non-interactable / is visible etc)
			// 4.1) Unit is owned
			if(hoveredUnit->GetOwner() == clientPlayer)
			{
				_cursorName = "TargetFriendly";
				hoverTarget->SetTargetType(HoverTargetTypes::OwnUnit);
			}
			// 4.2) Unit is allied
			else if(GameWorld::GlobalWorld->GetPlayers()->IsAlly(clientPlayer, hoveredUnit->GetOwner()))
			{
				_cursorName = "TargetFriendly";
				hoverTarget->SetTargetType(HoverTargetTypes::AllyUnit);
			}
			// 4.3) Unit is enemy
			else if(GameWorld::GlobalWorld->GetPlayers()->IsEnemy(clientPlayer, hoveredUnit->GetOwner()))
			{
				_cursorName = "TargetHostile";
				hoverTarget->SetTargetType(HoverTargetTypes::EnemyUnit);
			}
			// 4.4) Unit is neutral
			else if(GameWorld::GlobalWorld->GetPlayers()->IsNeutral(clientPlayer, hoveredUnit->GetOwner()))
			{
				_cursorName = "TargetNeutral";
				hoverTarget->SetTargetType(HoverTargetTypes::NeutralUnit);
			}
		}
	}
}
