#include "TrainingQueue.h"
#include "Unit.h"

#include "GameWorld.h"
#include "Resources.h"
#include "NoticeMessages.h"

namespace FlagRTS
{
	TrainingQueue::TrainingQueue(Unit* owner) :
		_currentUnit(0),
		_paused(false),
		_loopQueue(false),
		_maxSize(5),
		_owner(owner)
	{

	}

	TrainingQueue::~TrainingQueue()
	{
		_ASSERT( _currentUnit == 0 );
		if(_currentUnit != 0)
		{
			// Unit was still training when it was killed -> abort training
			// ( it should be done when despawned )
			Clear();
		}
	}

	void TrainingQueue::Update(float ms)
	{
		// Update only when trainer is in UnitTrainingState
		// Add progress to unit and check if its done
		if(_currentUnit != 0)
		{
			if( _currentUnit->GetConstructionProgress() >= 1.f )
			{
				// Unit is done -> try to spawn it
				if( _owner->TryFinishTrain( _currentUnit ) == true  )
				{
					_onUnitTrained.Fire(_owner, _currentUnit);
					// Unit was successfully spawned there is another unit on queue - switch to it
					TrainNext();
				}
				// else : do nothing
			}
			else if(_paused == false)
			{
				_currentUnit->SetConstructionProgress( _currentUnit->GetConstructionProgress() + 
					ms / _currentUnit->GetCurrentStats()->BuildTime);
			}
		}
	}

	void TrainingQueue::AddToQueue(UnitDefinition* definition)
	{
		// Check if there's enough space in training queue
		if( CheckIsSpaceInQueue() == false )
			return;

		// Check if player have enough resources
		auto& cost = definition->GetStats()->Cost;
		if(CheckPlayerHaveResources(cost) == false)
			return;

		// Take resources from player
		GameWorld::GlobalWorld->GetResources()->TakeResourcesFromPlayer(_owner->GetOwner(), cost);

		if( _currentUnit == 0 )
		{
			// Its first unit in queue :
			// create new unit and set is as currently training
			_currentUnit = static_cast<Unit*>(GameWorld::GlobalWorld->
				CreateSceneObject(definition, _owner->GetOwner()));
			_currentUnit->SetConstructionProgress(0.f);
			// Unit state was probably Idle - change to training, else changing to Training should be
			// ensured by current unit state
			if( _owner->GetCurrentState() == SceneObjectStates::Idle )
			{
				_owner->ChangeState(SceneObjectStates::Train);
			}
		}
		else
		{
			_queue.push_back(definition);
		}

		_onAddedToQueue.Fire(_owner, definition);
	}

	void TrainingQueue::RemoveFirstOfTypeFromQueue(UnitDefinition* definition, bool removeCurrent)
	{
		if( _currentUnit == 0 )
			return; // Queue empty

		if( removeCurrent && (definition->GetUnitKind() == _currentUnit->GetUnitKind()))
		{
			AbortTraining();
			return;
		}

		unsigned int curPos = 0;
		for(auto unitIt = _queue.begin(); unitIt != _queue.end(); ++unitIt)
		{
			if( definition->GetUnitKind() == unitIt->Value->GetUnitKind())
			{
				GameWorld::GlobalWorld->GetResources()->ReturnAllResourcesToPlayer(
					_owner->GetOwner(), unitIt->Value->GetStats()->Cost);

				UnitDefinition* def = unitIt->Value;
				_queue.erase(unitIt);
				_onQueuedUnitRemoved.Fire(_owner, def, curPos+1);
				return;
			}
			++curPos;
		}
	}

	void TrainingQueue::RemoveLastOfTypeFromQueue(UnitDefinition* definition, bool removeCurrent)
	{
		if( _currentUnit == 0 )
			return; // Queue empty

		unsigned int curPos = _queue.size();
		for(auto unitIt = _queue.end(); unitIt != _queue.begin();)
		{
			--unitIt;
			--curPos;
			if( definition->GetUnitKind() == unitIt->Value->GetUnitKind())
			{
				GameWorld::GlobalWorld->GetResources()->ReturnAllResourcesToPlayer(
					_owner->GetOwner(), unitIt->Value->GetStats()->Cost);

				UnitDefinition* def = unitIt->Value;
				_queue.erase(unitIt);
				_onQueuedUnitRemoved.Fire(_owner, def, curPos+1);
				return;
			}
		}

		if( removeCurrent && (definition->GetUnitKind() == _currentUnit->GetUnitKind()))
		{
			AbortTraining();
		}
	}

	void TrainingQueue::RemoveAt(uint32 pos)
	{
		_ASSERT( pos < _queue.size() );
		uint32 curPos = 0;
		for(auto unitIt = _queue.begin(); unitIt != _queue.end(); ++unitIt)
		{
			if( curPos == pos )
			{
				GameWorld::GlobalWorld->GetResources()->ReturnAllResourcesToPlayer(
					_owner->GetOwner(), unitIt->Value->GetStats()->Cost);

				UnitDefinition* def = unitIt->Value;
				_queue.erase(unitIt);
				_onQueuedUnitRemoved.Fire(_owner, def, curPos+1);
				return;
			}
			++curPos;
		}
	}

	void TrainingQueue::Clear()
	{
		// Return resources for every queued unit
		auto& cost = _currentUnit->GetCurrentStats()->Cost;
		auto resources = GameWorld::GlobalWorld->GetResources();
		resources->ReturnAllResourcesToPlayer(_owner->GetOwner(), cost);

		for(auto unitIt = _queue.begin(); unitIt != _queue.end(); ++unitIt)
		{
			auto& costQueued = unitIt->Value->GetStats()->Cost;
			resources->ReturnAllResourcesToPlayer(_owner->GetOwner(), cost);
		}
	}

	void TrainingQueue::Pause()
	{
		_paused = true;
	}

	void TrainingQueue::Resume()
	{
		_paused = false;
	}

	void TrainingQueue::AbortTraining()
	{
		// Destory current unit, return resources
		if( _currentUnit != 0 )
		{
			GameWorld::GlobalWorld->GetResources()->ReturnAllResourcesToPlayer(
				_owner->GetOwner(), _currentUnit->GetCurrentStats()->Cost);
			
			UnitDefinition* def = _currentUnit->GetUnitDefinition();
			GameWorld::GlobalWorld->DestroySceneObject(_currentUnit);
			_currentUnit = 0;
			_onQueuedUnitRemoved.Fire(_owner, def, 0);
		}
		TrainNext();
	}

	inline bool TrainingQueue::CheckIsSpaceInQueue()
	{
		if( GetQueueSize() >= GetMaxSize() )
		{
			GameWorld::GlobalWorld->ShowQuickNotice(NoticeMessage(
				Notices::GameError, (string("Queue full")).c_str(), _owner->GetOwner()));
			return false;
		}
		return true;
	}

	inline bool TrainingQueue::CheckPlayerHaveResources(const Array<ResourceCost>& cost)
	{
		uint8 lackingResource = 0;
		auto resources = GameWorld::GlobalWorld->GetResources();
		bool haveResources = resources->CheckPlayerHaveSufficientResources(
			_owner->GetOwner(), cost, &lackingResource);

		if( haveResources == false )
		{
			GameWorld::GlobalWorld->ShowQuickNotice(NoticeMessage(
				Notices::GameError, (string("Insuficient resource: ") + 
				resources->GetResourceName(lackingResource)).c_str(),
				_owner->GetOwner()));
			return false;
		}
		return true;
	}

	inline void TrainingQueue::TrainNext()
	{
		if( _queue.size() > 0u )
		{
			_currentUnit = static_cast<Unit*>(
				GameWorld::GlobalWorld->CreateSceneObject(_queue.front(), _owner->GetOwner()));
			_currentUnit->SetConstructionProgress(0.f);
			_queue.pop_front();
			if( _owner->GetCurrentState() == SceneObjectStates::Idle )
			{
				_owner->ChangeState(SceneObjectStates::Train);
			}
		}
		else
			_currentUnit = 0;
	}
}