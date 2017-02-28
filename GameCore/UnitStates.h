#pragma once

#include "SceneObjectState.h"
#include "UnitAIState.h"
#include "DataTypes.h"
#include <Event.h>

namespace FlagRTS
{
	class Unit;
	class Weapon;
	class Command;
	class CommandTarget;

	// Base state for all units
	// TODO: Remove CommandQueue update from states (or not?)
	class UnitState : public SceneObjectState
	{
	protected:
		Unit* _owner;

	public:
		UnitState(Unit* owner, size_t type, const char* name);

		void Update(float ms);
	};

	// Plays (Idle,"Default") animation and (Idle,"Bored") after "Default" played 3 times
	class UnitIdleState : public UnitState
	{
	protected:
		float _boredTimer;
		float _defaultActionLength;
		float _boredActionLength;

	public:
		UnitIdleState(Unit* owner);

		void Begin();
		void End();
		void Interrupt();
		void Resume();
		void Update(float ms);
	};

	// Plays (Move,"Default") animation in normal stituation
	// TODO If unit have slowing effect applied plays (Move,"Slow") animation
	// and if is boosted (Move,"Haste")
	// Executes actual movement following path from PathingSystem
	class UnitMoveState : public UnitState
	{
	public:
		UnitMoveState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
		void Interrupt();
		void Resume();
	};

	// Plays (Move,"Default") animation in normal stituation
	// Unlike default move state, it does not follow path from PathingSystem
	// but moves directly to target using current move strategy
	// Intended for internal use for some commands
	class UnitForceMoveState : public UnitState
	{
	protected:
		Vector3 _target;

	public:
		UnitForceMoveState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
		void Interrupt();
		void Resume();

		const Vector3& GetTarget() const { return _target; }
		void SetTarget(const Vector3& target) { _target = target; }
	};

	// Uses active weapons to attack targets ( weapons/target are
	// set in UnitWeaponSet ) 
	// Plays all animation of used weapons
	// (Attack, "WeaponAnimActionName")
	// If unit needs to face target its should be ensured before
	// changing state to attack
	class UnitAttackState : public UnitState
	{
	protected:
		bool _playingAnim; // If true attack animation is being played
		bool _weaponUsed; // Weapon was used in current animation

	public:
		UnitAttackState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
		void Interrupt();
		void Resume();
	};

	// For buildings with doors / gates or containers etc
	// Plays (Internal,"Open")
	class UnitOpeningState : public UnitState
	{
	public:
		UnitOpeningState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
		void Interrupt();
		void Resume();
	};

	// For buildings with doors / gates or containers etc
	// Plays (Internal,"Close")
	class UnitClosingState : public UnitState
	{
	public:
		UnitClosingState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
		void Interrupt();
		void Resume();
	};

	// Plays (Dying,"Default") animation
	// When animation ends it despawns unit
	class UnitDyingState : public UnitState
	{
	protected:
		bool _finished;

	public:
		UnitDyingState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
		void Interrupt();
		void Resume();
	};

	// Standard OnSpawned state for buildings, must be changed to idle or anything
	// by construction system
	class BuildingUnderConstructionState : public UnitState
	{
	public:
		BuildingUnderConstructionState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
		void Interrupt();
		void Resume();
	};

	// In building state its draining resources and builds the building
	class ConstructionSiteBuildingState : public UnitState
	{
		float _tickTimer;
		float _tickBuildFraction; // Fraction of total build time of 1 tick
		float _buildingHeight;
		int _constructionTime;

	public:
		ConstructionSiteBuildingState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
		void Interrupt();
		void Resume();

	private:
		void OnBuildingDestroyed(Unit* building);
		DEFINE_DELEGATE1(BuildingDestroyedDelegate, ConstructionSiteBuildingState,
			OnBuildingDestroyed, Unit*);
		BuildingDestroyedDelegate _onBuildingDestroyed;
	};

	// Default state for unit trainers -> updates training queue + plays "Train" animation
	// If train finished spawns unit in adjacent free cell closest to rally point 
	// ( in TryFinishTrain() function called by TrainingQueue )
	// If spawning failed TrainingQueue() will try to spawn it every update
	// All other states must derive from this state, as TryFinishTrain() function is
	// neccessary for training queue
	class UnitTrainingState : public UnitState
	{
	public:
		UnitTrainingState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
		void Interrupt();
		void Resume();

		virtual bool TryFinishTrain(Unit* trainedUnit);
	};
}