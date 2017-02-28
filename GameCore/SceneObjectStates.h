#pragma once

#include "SceneObjectStateMachine.h"

namespace FlagRTS
{
	// Basic SO idle state does just nothing
	class SceneObjectIdleState : public SceneObjectState
	{
	public:
		SceneObjectIdleState() :
			SceneObjectState(SceneObjectStates::Idle, "Idle")
		{ }

		void Update(float ms) { }

	};
	
	// Basic SO ns state does just nothing
	class SceneObjectNotSpawnedState : public SceneObjectState
	{
	public:
		SceneObjectNotSpawnedState() :
			SceneObjectState(SceneObjectStates::NotSpawned, "NotSpawned")
		{ }

		void Update(float ms) { }
	};

	// Basic SO unknown state does just nothing
	// Unknowns state is set when wrong state was choosen (or not used at all)
	class SceneObjectUnknownState : public SceneObjectState
	{
	public:
		SceneObjectUnknownState() :
			SceneObjectState(SceneObjectStates::Unknown, "Unknown")
		{ }

		void Update(float ms) { }
	};
}