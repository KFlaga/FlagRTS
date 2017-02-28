#pragma once

#include "SceneObjectState.h"
#include <map>
#include <Array.h>

namespace FlagRTS
{
	class SceneObjectState;

	/**
	TODO: change SOStates so that they comply with below notes
	TODO: as for now only one Pop() per may update occur -> change it

	State machine works as state stack-queue hybrid:
	- when new state is requested, then it is pushed to top
	- if current state may be interrupted, then new state is started, until we reach the top and top state is used (stack-like behavior)
	- if state cannot be interrupted, then changing states waits for current state to change (queue-like behavior)
	- when new state finishes it is poped and previous state is resumed
	- if there was no previous state left, default one is started
	- every frame current state status is checked and status of next state in stack
	- depending on this status state is popped / interrupted / nothing is done
	- change of state can be forced by reseting whole stack

	Before state is started Begin is called, then each frame it is Updated,
	when it is paused/resumed Interrupt/Resume is called. If it is to be popped
	then End is called.

	By default SceneObject starts with state "NotSpawned" (mandatory state)
	When it spawns changes to "Spawning" if there's such state and if there is not
	changes to "Idle" (mandatory state).
	If object finishes its state's actions it moves back to previous state.
	If object state was poped and there is no previous state, then it changes to "Idle".
	"Idle" and "NotSpawned" cannot have "Preparation", "RunningCritical" and "Finished"  status.

	State machine owns all States which are xDeleted with Machine
	*/
	class SceneObjectStateMachine
	{
	protected:
		typedef std::map<size_t, SceneObjectState*> StateMap;
		StateMap _states;

		Array<SceneObjectState*> _stateStack;
		int _currentIndex;

	public:
		SceneObjectStateMachine();
		~SceneObjectStateMachine();

		/** 
		Checks if state should be changed.
		Updates state (after changes).
		*/
		void Update(float ms);

		/* 
		Adds new state and registers it as accessbile with SceneObjectState::GetType()
		If same id is already used, old one is xDeleted and new one is inserted. 
		*/
		void AddState(SceneObjectState* state);

		/* 
		Removes state with given id and xDeletes it.
		*/
		void RemovesState(size_t id);

		/**
		Puts given state on top of stack -> it will be changed to as soon as
		previously pushed states allow it.
		If there's no state with given id does nothing
		*/
		void PushState(size_t stateId);

		/**
		Resets stack and pushes state with given id - so on stack there
		will be "Idle" and given state, unless given state is "Idle" or
		"NotSpawned", then there will be only this one state.
		If there's no state with given id does nothing.
		*/
		void ChangeState(size_t stateId);

		/**
		Resets state stack, so that theres only "Idle" state on top.
		Calls 'End()' on each state on stack that have no "Preparation" or "Ready" status
		from top to bottom
		*/
		void ResetStack();

		/** Returns state with given key, or 0 if there no such state */
		SceneObjectState* FindState(size_t stateId);

		SceneObjectState* GetCurrentState() const
		{
			return _stateStack[_currentIndex];
		}

		size_t GetCurrentStateId() const 
		{ 
			return _stateStack[_currentIndex]->GetType();
		}

	protected:
		void ResetStack(SceneObjectState* defaultState);
		void NextState();
		void PopState();
		void SwitchToPreviousState();
	};
}