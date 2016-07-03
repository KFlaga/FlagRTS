#pragma once

#include "SceneObjectStateMachine.h"

namespace FlagRTS
{
	// Basic SO idle state does just nothing
	class SceneObjectIdleState : public SceneObjectState
	{
	public:
		void Update(float ms) { }

	};
	
	// Basic SO ns state does just nothing
	class SceneObjectNotSpawnedState : public SceneObjectState
	{
	public:
		void Update(float ms) { }
	};

	// Basic SO unknown state does just nothing
	// Unknowns state is set when wrong state was choosen
	class SceneObjectUnknownState : public SceneObjectState
	{
	public:
		void Update(float ms) { }
	};
}