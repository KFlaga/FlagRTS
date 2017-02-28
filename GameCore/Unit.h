#pragma once

#include "UnitDefinition.h"
#include "PhysicalObject.h"
#include "UnitStats.h"
#include "CommandQueue.h"
#include "IMoveStrategy.h"

namespace FlagRTS
{
	class IMoveStrategy;
	class Command;
	class SceneMarker;
	class Weapon;

	class Unit : public PhysicalObject
	{
	protected:
		UnitDefinition* _unitDef;
		CommandQueue _commands;
		UnitStats _currentStats;
		IMoveStrategy* _moveStrategy; // Strategy used to move this unit
		UnitClass _finalUnitClass; // Final unit class defines : unit movement type (ground/air/building etc) 
							 // and intended use (aggresive/worker/factory etc) combined in binary flags set

		Vector3 _rallyPoint; // If unit can train it contains rally point for newly trained units

		Event<Unit*> _onDied; // Event called when unit dies

		bool _isInteractable; // Enable/Disable user commands

		Event<Unit*, Unit*, bool> _onConstructionFinsished;
		float _constructionProgress; // Applies if under construction
		bool _isUnderConstruction; // Building/mobile is constructing/training

		bool _aiEnabled; // Enable/Disable all unit auto behaviour

		bool _isAlive; // Additional flag -> set to false if unit is killed ( to not be killed twice )
		// Possible unit disables
		bool _isImmobilized; // Unit cannot move
		bool _isDisarmed; // Unit cannot attack
		bool _isSilienced; // Unit cannot use ability type commands

	public:
		Unit(UnitDefinition* unitDef );
		~Unit();

		UnitDefinition* GetUnitDefinition() const
		{ 
			return _unitDef;
		}

		void LoadResources(Ogre::SceneManager* ogreMgr);
		void UnloadResources(Ogre::SceneManager* ogreMgr);

		void Spawn();
		void Despawn();

		void Update(float ms);

		UnitClass GetUnitClass() const { return _finalUnitClass; }
		void SetUnitClass(UnitClass uclass);
		void AddSubClassToUnit(UnitClass subclass) { SetUnitClass(_finalUnitClass | subclass); }

		// Unit kind type is determined by its definition handle
		size_t GetUnitKind() const { return _unitDef->GetUnitKind(); }

		// Executes command of given type / ability with hotkey and given target
		// If queue = false, current command will be interrupted and new one started, 
		// if true it will be added to command queue
		void ExecuteCommand(int commandType, CommandTarget* target, bool queue = false)
		{
			_commands.ExecuteCommand(commandType, target, queue);
		}

		CommandQueue* GetCommandQueue() { return &_commands; }
		UnitCommandSet* GetCommands() { return &_commands.GetCommands(); }

		void SetAIEnabled(bool value);
		bool GetAIEnabled() const { return _aiEnabled; }

		IMoveStrategy* GetMoveStrategy() const { return _moveStrategy; }
		// Sets new move strategy for unit
		void SetMoveStrategy(IMoveStrategy* strategy) 
		{ 
			_moveStrategy = strategy; 
		}
		// Sets default move strategy ( from definition )
		void SetDefaultMoveStrategy() 
		{ 
			_moveStrategy = _unitDef->GetMoveStrategy(); 
		}
		// Sets move strategy with given name ( only if found )
		void SetMoveStrategy(const string& name);

		// Moves unit using current strategy
		void MoveUnit(const Vector3& target, const float ms)
		{ 
			_moveStrategy->Move(this, target, ms);
		}

		const string& GetGuiName() const { return GetUnitDefinition()->GetGuiName(); }
		bool CountsAsPlayerUnit() const { return GetUnitDefinition()->CountsAsPlayerUnit(); }

#pragma region UnitState
		// Event that occurs when units was under construction/training and just finished
		// First argument is builder ( construction site / training building ), second finished building, 3rd if construction was a success of was aborted
		// Builder is 0 if finished unit have no knowledge of its builder
		Event<Unit*, Unit*, bool>& ConstructionFinished()  { return _onConstructionFinsished; }

		// Substract unit's health by given amout, checks if unit died,
		// notify AI it is attacked to return fire if applicable etc.
		void InflictDamage(int damage, Weapon* damageSource);

		// TODO
		// Applies given status effect to unit, modifing its stats/state etc.
		// Also notifies AI in case it should take some action
		void ApplyStatus();

		// Kills this unit (sets hp = 0), fires UnitDied() event
		// Reason must be specified + if 'instant' = true, dying animation is not played ( unit is despawned immediately )
		void KillUnit(int reason, bool instant = false);

		Event<Unit*>& UnitDied() { return _onDied; }

		bool CanGarnizon() const { return false; }
		bool HaveFreeGranizon() const { return false; }

		// If unit is interactable user can order commands to it
		// Else only Internal commands will be executed
		bool IsInteractable() const { return _isInteractable; }
		void SetIsInteractable(const bool value) { _isInteractable = value; }

		bool IsDead() const { return !_isAlive; }
		bool IsAlive() const { return _isAlive; }

		bool IsUnderConstruction() const { return _isUnderConstruction; }
		void SetIsUnderConstruction(bool value) { _isUnderConstruction = value; }

		float GetConstructionProgress() const { return _constructionProgress; }
		void SetConstructionProgress(float value) { _constructionProgress = value; }

		const Vector3& GetRallyPoint() const { return _rallyPoint; }
		void SetRallyPoint(const Vector3& point) { _rallyPoint = point; }
		
		bool IsImmobilized() const { return _isImmobilized; }
		void SetIsImmobilized(bool value) { _isImmobilized = value; }
		
		bool IsDisarmed() const { return _isDisarmed; }
		void SetIsDisarmed(bool value) { _isDisarmed = value; }
		
		bool IsSilienced() const { return _isSilienced; }
		void SetIsSilienced(bool value) { _isSilienced = value; }

#pragma endregion

#pragma region UnitStats
		const UnitStats* GetCurrentStats() const { return &_currentStats; }
		UnitStats* GetCurrentStats() { return &_currentStats; }
		const UnitStats* GetBaseStats() const { return _unitDef->GetStats(); }
		UnitStats* GetBaseStats() { return _unitDef->GetStats(); }

		int GetHitPoints() const { return _currentStats.HitPoints; }
		void SetHitPoints(int value) { _currentStats.HitPoints = value; }
		void AddHitPoints(int value) { _currentStats.HitPoints += value; }

		int GetMaxHitPoints() const { return _currentStats.MaxHitPoints; }
		void SetMaxHitPoints(int value) { _currentStats.MaxHitPoints = value; }
		void AddMaxHitPoints(int value) { _currentStats.MaxHitPoints += value; }

		float GetSpeed() const { return _currentStats.MoveSpeed; }
		void SetSpeed(float value) { _currentStats.MoveSpeed = value; }
		void AddSpeed(float value) { _currentStats.MoveSpeed += value; }

		float GetRotationSpeed() const { return _currentStats.RotateSpeed; }
		void SetRotationSpeed(float value) { _currentStats.RotateSpeed = value; }
		void AddRotationSpeed(float value) { _currentStats.RotateSpeed += value; }

		const Array<ResourceCost>& GetResourceCost() const { return _currentStats.Cost; }
		Array<ResourceCost>& GetResourceCost() { return _currentStats.Cost; }

		float GetBuildTime() const { return _currentStats.BuildTime; }
		void SetBuildTime(float value) { _currentStats.BuildTime = value; }


#pragma endregion
	};
}