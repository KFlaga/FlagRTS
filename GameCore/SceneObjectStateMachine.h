#pragma once

#include "SceneObjectState.h"
#include <map>

namespace FlagRTS
{
	class SceneObjectState;

	// Every scene object have own state machine
	// which manages its state and delegate interaction
	// to current state
	class SceneObjectStateMachine
	{
	protected:
		typedef std::map<size_t, SceneObjectState*> StateMap;
		StateMap _states;
		size_t _currentStateId;
		SceneObjectState* _currentState;

	public:
		SceneObjectStateMachine();
		~SceneObjectStateMachine();

		void Update(float ms) { _currentState->Update(ms); }

		// Adds new state with given id. If same id is already used, old one is xDeleted and
		// new one is inserted. ( So if default state should be overriden by custom one
		// custom one should be added later )
		void AddState(size_t id, SceneObjectState* state);

		// Changes current state of object 
		void ChangeState(size_t newState);

		// Returns state with given key, or 0 if there no such state
		SceneObjectState* FindState(size_t state);

		size_t GetCurrentState() const 
		{ 
			return _currentStateId;
		}

		SceneObjectState* GetCurrentStatePtr() const 
		{ 
			return _currentState;
		}
	};
}