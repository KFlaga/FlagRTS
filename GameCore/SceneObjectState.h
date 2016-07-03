#pragma once

#include <TypeDefs.h>

namespace FlagRTS
{
	namespace SceneObjectStates
	{
		enum StateType : size_t
		{
			Unknown = 0,
			NotSpawned,
			OnSpawn,
			OnDespawn,
			Idle,
			Move,
			Attack,
			AttackMove,
			Cast,
			Channel,
			Stun,
			Other,
			Internal,
			ForceMove,
			UnderConstruction,
			Train,
			Research,
			Open,
			Close,
			Dying,
			StateTypesCount
		};

		StateType ParseStateType(const char* name);
	}
	typedef SceneObjectStates::StateType SceneObjectStateType;

	// Scene object states describes current state of scene object -
	// in most cases its current activity ( like moving, idle )
	// Scene objects are updated via states - so they are
	// responsible for updating SO according to current state
	// With every state usually comes animation played on
	// begining if SO is animating object
	// SceneObject itself provide basic Idle and NotSpawned
	// states used if object do not needs any updates
	class SceneObjectState
	{
	public:
		SceneObjectState() { }

		virtual void Begin() { }
		
		virtual void End() { }

		virtual void Update(float ms) = 0;

		virtual const char* GetName()
		{
			return "Unknown";
		}

		virtual size_t GetType()
		{
			return SceneObjectStates::Unknown;
		}
	};
}