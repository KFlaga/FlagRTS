#include "SceneObjectStateMachine.h"
#include "SceneObject.h"
#include <map>

namespace FlagRTS
{
	SceneObjectStateMachine::SceneObjectStateMachine() :
		_currentState(0)
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

	void SceneObjectStateMachine::ChangeState(size_t newState)
	{
		auto stateIt = _states.find(newState);
		if(stateIt != _states.end())
		{
			if(_currentState != 0)
				_currentState->End();
			_currentState = stateIt->second;
			_currentStateId = newState;
			_currentState->Begin();
		}
	}

	void SceneObjectStateMachine::AddState(size_t id, SceneObjectState* state)
	{
		auto stateIt = _states.find(id);
		if(stateIt != _states.end())
		{
			xDelete(stateIt->second);
		}
		_states[id] = state;
	}

	SceneObjectState* SceneObjectStateMachine::FindState(size_t state)
	{
		auto stateIt = _states.find(state);
		if(stateIt != _states.end())
		{
			return stateIt->second;
		}
		return 0;
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
			map["Cast"] = Cast;
			map["Channel"] = Channel;
			map["Stun"] = Stun;
			map["Other"] = Other;
			map["Internal"] = Internal;
			map["Train"] = Train;
			map["Research"] = Research;
			map["Open"] = Open;
			map["Close"] = Close;
			map["Dying"] = Dying;
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