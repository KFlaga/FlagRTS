#include "SceneObjectStateMachine.h"
#include "SceneObject.h"
#include <map>
#include <Exception.h>

namespace FlagRTS
{
	SceneObjectStateMachine::SceneObjectStateMachine() :
		_currentIndex(-1)
	{

	}

	SceneObjectStateMachine::~SceneObjectStateMachine()
	{
		// First remove duplicates
		for(auto stateIt = _states.begin(); stateIt != _states.end(); ++stateIt)
		{
			auto state2It = stateIt;
			++state2It;
			while(state2It != _states.end())
			{
				if(stateIt->second == state2It->second)
					state2It->second = 0;
				++state2It;
			}
			xDeleteSafe(stateIt->second);
		}

		_states.clear();
	}

	void SceneObjectStateMachine::Update(float ms)
	{
		StateStatus currStatus = GetCurrentState()->CheckStatus();
		if(_currentIndex == _stateStack.size() - 1) // There is no next state
		{
			switch(currStatus)
			{
			//case StateStatus::RunningCritical:
			//case StateStatus::RunningNoncritical:
			//	break;
			case StateStatus::Ready:
				GetCurrentState()->Begin();
				break;
			case StateStatus::Paused:
				GetCurrentState()->Resume();
				break;
			case StateStatus::Finished:
				PopState();
				break;
			case StateStatus::Preparation:
				// WELL there should be no such sitation
				// But we can switch to previous state
				SwitchToPreviousState();
				break;
			}
		}
		else // There's pushed state awaiting
		{
			switch(currStatus)
			{
			//case StateStatus::RunningCritical:
			//	break;
			case StateStatus::RunningNoncritical:
			case StateStatus::Ready:
			case StateStatus::Paused:
			case StateStatus::Finished:
				NextState();
				break;
			case StateStatus::Preparation:
				// WELL there should be no such sitation
				// But we can switch to previous state
				SwitchToPreviousState();
				break;
			}
		}
		GetCurrentState()->Update(ms);
	}

	void SceneObjectStateMachine::NextState()
	{
		_ASSERT(_stateStack.size() > _currentIndex + 1);

		// Interrupt previous State and start new one
		SceneObjectState* nextState = _stateStack[_currentIndex + 1];
		StateStatus nextStatus = nextState->CheckStatus();

		if(nextStatus == StateStatus::Preparation)
			return;

		if(nextStatus == StateStatus::Ready)
		{
			// If current state was finished then remove it, else pause
			if(GetCurrentState()->CheckStatus() == StateStatus::Finished)
			{
				GetCurrentState()->End();
				_stateStack.removeAt(_currentIndex);
			}
			else
			{
				GetCurrentState()->Interrupt();
				_currentIndex += 1;
			}
			nextState->Begin();
			
			// If there are more states pushed, start with top one
			if(_stateStack.size() > _currentIndex + 1)
			{
				NextState();
			}
		}

		CastException_d(string("Invalid status of sstate: ") + nextState->GetName());
		// Return to defualt state
		ResetStack();
	}

	void SceneObjectStateMachine::PopState()
	{
		// We should pop only if top state is current one
		_ASSERT(_currentIndex == _stateStack.size() - 1);

		// Try to remove top state from stack
		StateStatus topStatus = GetCurrentState()->CheckStatus();

		if(topStatus == StateStatus::RunningCritical)
		{
			// We cannot pop it - do nothing
			return;
		}

		// Otherwise we can end it and pop
		GetCurrentState()->End();
		_stateStack.pop_back();

		_currentIndex -= 1;
	}

	void SceneObjectStateMachine::SwitchToPreviousState()
	{
		CastException_d(string("Invalid status of state: ") + GetCurrentState()->GetName());
		// Generally we should not be there
		_currentIndex -= 1;
	}

	void SceneObjectStateMachine::PushState(size_t stateId)
	{
		SceneObjectState* newState = FindState(stateId);
		if(newState != 0)
		{
			_stateStack.push_back(newState);
		}
	}

	void SceneObjectStateMachine::ChangeState(size_t stateId)
	{
		SceneObjectState* newState = FindState(stateId);
		if(newState != 0)
		{
			if(stateId == SceneObjectStates::Idle ||
				stateId == SceneObjectStates::NotSpawned)
			{
				// Change to Idle or NotSpawned -> only this state is on
				ResetStack(newState);
			}
			else
			{
				// Change to other state -> Idle and new state is on
				ResetStack();

				GetCurrentState()->Interrupt();
				_stateStack.push_back(newState);
				newState->Begin();
				_currentIndex = 1;
			}
		}
	}

	void SceneObjectStateMachine::AddState(SceneObjectState* state)
	{
		auto stateIt = _states.find(state->GetType());
		if(stateIt != _states.end())
		{
			xDelete(stateIt->second);
		}
		_states[state->GetType()] = state;
	}

	void SceneObjectStateMachine::RemovesState(size_t id)
	{
		auto stateIt = _states.find(id);
		if(stateIt != _states.end())
		{
			xDelete(stateIt->second);
		}
	}

	SceneObjectState* SceneObjectStateMachine::FindState(size_t state)
	{
		auto stateIt = _states.find(state);
		return stateIt != _states.end() ? stateIt->second : 0;
	}

	void SceneObjectStateMachine::ResetStack()
	{
		ResetStack(FindState(SceneObjectStates::Idle));
	}

	void SceneObjectStateMachine::ResetStack(SceneObjectState* defaultState)
	{
		// Call End() on all touched states
		int idx = _stateStack.size() - 1;
		while(idx > 0)
		{
			SceneObjectState* state = _stateStack[idx];
			StateStatus status = state->CheckStatus();
			if(!(status == StateStatus::Preparation ||
				status == StateStatus::Ready))
			{
				state->End();
			}
			--idx;
		}

		_stateStack.clear();
		_stateStack.push_back(defaultState);
		_currentIndex = 0;

		GetCurrentState()->Begin();
	}

	namespace SceneObjectStates
	{
		std::map<string, StateType>& GetStateNamesMap()
		{
			static std::map<string, StateType> map;
			map["Unknown"] = Unknown;
			map["NotSpawned"] = NotSpawned;
			map["OnSpawn"] = OnSpawn;
			map["OnDespawn"] = OnDespawn;
			map["Idle"] = Idle;
			map["Move"] = Move;
			map["Attack"] = Attack;
			map["AttackMove"] = AttackMove;
			map["Other"] = Other;
			map["Internal"] = Internal;
			map["Research"] = Research;
			map["Open"] = Open;
			map["Close"] = Close;
			map["Dying"] = Dying;

			map["Playing"] = Playing;
			map["Stopped"] = Stopped;
			map["Paused"] = Paused;

			map["Training"] = Training;
			map["TrainingStart"] = TrainingStart;
			map["TrainingPrepare"] = TrainingPrepare;
			map["TrainingFinished"] = TrainingFinished;
			return map;
		}

		StateType ParseStateType(const char* name)
		{
			static std::map<string, StateType>& map = GetStateNamesMap();
			auto typeIt = map.find(name);
			if(typeIt != map.end())
			{
				return typeIt->second;
			}
			return Unknown;
		}
	}
}