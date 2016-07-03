#include "IObjectStateSupplier.h"

namespace FlagRTS
{
	// Adds default not-spawned and idle states
	class MapDecoratorNoPrespawnStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	// For classless game camera 
	class GameCameraDefaultStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	// Adds default not-spawned / idle / dying state
	class UnitBaseStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	// Adds idle state scanning for enemy
	// Plays action='Default' animation + action='Bored' animations
	class UnitAggresiveStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	// Adds idle state hold ground
	// Plays action='Default' animation + action='Bored' animations
	class UnitPassiveStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	// Adds default move state for ground unit (UnitMoveState + UnitForceMoveState)
	class UnitMoveGroundStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};
	
	// Adds ForceMove state
	class UnitRoteteableStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	// Adds default attack ground state
	// Plays action='AttackMelee','AttackRanged' based on weapon used
	// During cooldown plays action='AttackMeleeCooldown','AttackRangedCooldown' 
	class UnitAttackGroundStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	// Adds OnSpawn state to unit = BuildingUnderConstructionState
	class BuildingStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	// Adds OnSpawn + Idle states for ConstructionSite
	class ConstructionSiteStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	// Adds default not-spawned and idle states
	class SceneMarkerDefaultStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	// Adds default not-spawned and idle states
	class MissleDefaultStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};
}