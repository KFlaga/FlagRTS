#pragma once

#include <List.h>
#include "UnitStats.h"
#include <Event.h>

namespace FlagRTS
{
	class Unit;
	class UnitDefinition;

	// TrainingQueue is used for unit training -> handles to definitions are
	// pushed back and units are trained one after another
	// If there are insufficient resources training of next unit will
	// hold utill there's some
	// If entrance is blocked ( that is _owner->TryFinishTrain(Unit*) == false )
	// finishing of current unit will hold
	class TrainingQueue
	{
	private:
		Unit* _owner;
		List<UnitDefinition*> _queue;
		Unit* _currentUnit;
		uint32 _maxSize;

		Event<Unit*, Unit*> _onUnitTrained; // Fired when unit is trained
		Event<Unit*, UnitDefinition*, uint32> _onQueuedUnitRemoved; // Fired when one of queued unit is removed
		Event<Unit*, UnitDefinition*> _onAddedToQueue;

		bool _paused;
		bool _loopQueue;

	public:
		TrainingQueue(Unit* owner);
		~TrainingQueue();

		void Update(float ms);

		Unit* GetOwner() { return _owner; }
		List<UnitDefinition*>& GetQueuedUnits() { return _queue; }
		const List<UnitDefinition*>& GetQueuedUnits() const { return _queue; }
		Unit* GetCurrentlyTrainedUnit() const { return _currentUnit; }
		// Returns number of queued units ( not counting currently trained one )
		uint32 GetQueueSize() const { return _queue.size(); }

		bool IsQueueLooped() const { return _loopQueue; }
		void SetQueueLoop(bool value) { _loopQueue = value; }

		uint32 GetMaxSize() const { return _maxSize; }
		void SetMaxSize(uint32 size) { _maxSize = size; }

		bool IsTraining() const
		{
			return _currentUnit != 0 || _queue.size() > 0; 
		}

		// Pushed unit to back of queue
		// If there's no space does nothing, sends notice
		// If player have not enough resources does nothing, sends notice
		void AddToQueue(UnitDefinition* definition);
		// Removes first unit of given type from queue
		// If 'removeCurrent' == true also currently trained one can be removed
		void RemoveFirstOfTypeFromQueue(UnitDefinition* definition, bool removeCurrent = false);
		// Removes last unit of given type from queue
		// If 'removeCurrent' == true also currently trained one can be removed
		void RemoveLastOfTypeFromQueue(UnitDefinition* definition, bool removeCurrent = false);
		// Removes unit from queue on given position
		void RemoveAt(uint32 pos);
		// Removes all units from queue ( including aborting currently trained one )
		void Clear();
		// Pauses training of unit if there's one being trained
		void Pause();
		// Resumes training of unit if it's paused
		void Resume();
		// Aborts training of unit if one is being trained and moves to next one
		// Returns resources used for training
		void AbortTraining();

		// Fired when unit is trained and spawned ( params : owner, trained unit )
		Event<Unit*, Unit*>& UnitTrained() { return _onUnitTrained; }
		// Fired when one of queued unit is removed
		// ( params : owner, removed unit, pos in queue (0 is CurrentUnit) )
		Event<Unit*, UnitDefinition*, uint32>& QueuedUnitRemoved() { return _onQueuedUnitRemoved; }
		// Fired when unit is successfully added to queue ( params : owner, added unit def )
		Event<Unit*, UnitDefinition*>& UnitAddedToQueue() { return _onAddedToQueue; }

	private:
		bool CheckIsSpaceInQueue();
		bool CheckPlayerHaveResources(const Array<ResourceCost>& cost);
		void TrainNext();
	};
}