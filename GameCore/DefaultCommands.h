#pragma once

#include "Command.h"
#include <Event.h>
#include "DefaultMoveStrategies.h"

namespace FlagRTS
{
	class Weapon;
	class SceneObject;
	class Unit;
	class IMoveStrategy;

	// Command to simply to nothing, when unit should ignore given command type
	class DoNothingCommand : public Command
	{
	public:
		DoNothingCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		Command* GetCopy();
	};

	// Default move to point command for ground units
	// Uses Pathing system to find a way
	class BasicMoveGroundCommand : public Command
	{
		float _range; // Range within target point, object should end movement
		Vector3 _pointTarget;
		bool _moveToObject;

	public:
		BasicMoveGroundCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		void SetRange(float range) { _range = range; }

		bool IsTargetValid(CommandTarget* target) const;

	protected:
		void OnTargetDespawned(SceneObject* target)
		{
			_moveToObject = false;
		}
		DEFINE_DELEGATE1(TargetDespawnedEventHandler, BasicMoveGroundCommand,
			OnTargetDespawned, SceneObject*);
		TargetDespawnedEventHandler _onTargetDespawned;

	};

	// Uses formation for movement ( orders formation to move - command will be
	// executed only once first unit in formation )
	// Uses PathingSystem to find a way
	// Default movement command for units
	class GroupMoveGroundCommand : public Command
	{
	public:
		GroupMoveGroundCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
	};

	// Basic attack command does :
	// In Execute :
	// - check if target is within range, if yes chooses weapon and set Attack state
	// - unless target is on opposite direction -> then queue rotation
	// - if target is out of range then queue moving to target ( untill in range )
	// In Update :
	// - checks if target moves out of range / direction and if yes does same things as above
	// - if target died ends command
	class BasicAttackCommand : public Command
	{
	protected:
		Unit* _unitTarget;
		Weapon* _usedWeapon;
		bool _notAttacked;

	public:
		BasicAttackCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;

	protected:
		void OnUnitDied(Unit* unit);
		DEFINE_DELEGATE1(UnitDiedEventHandler, BasicAttackCommand,
			OnUnitDied, Unit*);
		UnitDiedEventHandler _onUnitDied;
	};

	// Basic roate command does - just rotates until reached target direction
	class BasicRotateCommand : public Command
	{
	protected:
		bool _rotateTowardsObejct;
		RotateOnlyStrategy _rotateStrategy;
		IMoveStrategy* _executorStrategy;

	public:
		BasicRotateCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;

	protected:
		void OnTargetDespawned(SceneObject* target)
		{
			_rotateTowardsObejct = false;
		}
		DEFINE_DELEGATE1(TargetDespawnedEventHandler, BasicRotateCommand,
			OnTargetDespawned, SceneObject*);
		TargetDespawnedEventHandler _onTargetDespawned;
	};

	// Aborts construction of building
	class AbortBuildingConstructionCommand : public Command
	{
	public:
		AbortBuildingConstructionCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
	};

	// Base command for unit training -> stores UnitDefinition of unit to be trained
	// and starts training in training queue
	class TrainUnitCommand : public Command
	{
		UnitDefinition* _trainedUnit;

	public:
		TrainUnitCommand(Unit* owner, UnitDefinition* trainedUnit, int trainNumber);
		// Gets definition from name through GameWorld
		TrainUnitCommand(Unit* owner, const char* trainedUnitName, int trainNumber);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
	};
}