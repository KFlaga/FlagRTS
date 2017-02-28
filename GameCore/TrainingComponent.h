#pragma once

#include <Array.h>
#include <Event.h>
#include <functional>
#include "IObjectComponent.h"
#include "ObjectDefinition.h"

namespace FlagRTS
{
	class Unit;
	class UnitDefinition;

	class TrainingComponentDefinition;

	/**
	Allows a unit (Trainer) to train other units (Trainee) (only Units can own this Component).
	By training is meant construction of mobile unit inside another unit, like construction of
	soldier inside barracks. Training is not:
	- construction of structures
	- construction of mobile units outside of Trainer
	- construction of extensions for Trainer or other unit

	Behavior of Component is customisable by set of parameters choosen globaly which may be overriden per ComponentDefinition.
	Result of training is always spawning a unit. Depending on flags before spawning Trainer may need
	to be 'free' (i.e. if factory doors opens, unit is spawned and then moves out of factory - 
	during 'open', 'move out', 'close' factory is not 'free'). Also after spawning Mover may be
	applied to unit which will move it out of Trainer spawn area (then Trainer is freed if Mover
	successfuly finshes) and/or move command to rally spot is ordered - or (ofc depending on flags)
	chain of commands previously set as default for every trained unit.

	When some train command is executed Unit is queued for training.
	As soon as there are resources available and previous unit finished training
	another unit starts to train.
	When training begins state 'TrainingPrepare' and 'Training' is pushed. 
	After unit is fully trained and successfully spawned 'TrainingFinished' state is pushed;
	If queued unit needed a requirement to be satisfied and this requirement is unsatisfied
	unit is removed from queue, but if it is already being trained nothing happens.

	TODO: add multiple training slots option
	*/

	class TrainingComponent : public IObjectComponent
	{
	public:
		/**
		Determines when resources are taken from player.
		*/
		enum PaymentMethod : int
		{
			/** Player must have resources to queue unit. Looping is disabled in this mode. */
			PaidInAdvance = 0,
			/** Player must have resources to queue unit excluding supply-type resources
			which are taken	when unit starts training. Looping is disabled in this mode. */
			PaidInAdvance_NoSupply = 0,
			/** Resources are taken when unit's training starts - 
			if player have insufficent resources then Component waits for resources.*/
			WaitForResources,
			/** Resources are exhausted at constant rate during training excluding supply-type resources
			which are taken	when unit starts training.  */
			PaidContinually, 
		};

	public:
		/**
		Determines what to do when units finished training
		*/
		enum SpawnBehavior
		{
			/**	
			Spawns unit adjacently to Trainer in nearset possible location even if Trainer is blocked 
			from each side - unit will spawn in closest free big enough spot on map.
			After training state and before actual spawning 'TrainingFinished' is pushed 
			(plays animation 'Training::Finished' if Trainer have such, otherwise does nothing and ends immediately)
			*/
			SpawnOutsideFar = 0,	
			/**	
			Spawns unit adjacently to Trainer in nearset possible location but only if there is free spot 
			close to Trainer (at most as far as value 'MaxSpawnDistance' or	adjacently to Trainer).
			After training state and before actual spawning 'TrainingFinished' is pushed 
			(plays animation 'TrainingFinished' if Trainer have such, otherwise does nothing and ends immediately)
			*/
			SpawnOutsideNear,
			/**	
			Spawns unit in 'SpawnSpot' (location relative to Trainer position).
			Before spawning Trainer must be in 'free' state.
			After training 'TrainingPrepareSpawn', 'TraineeMovingOut', 'TrainingFinished' states are pushed for Trainer :
			- 'TrainingPrepareSpawn' : plays animation 'Training::PrepareSpawn' if Trainer have such, 
			otherwise does nothing and ends immediately
			- 'TraineeMovingOut' : plays looped animation 'Training::TraineeMovingOut' if Trainer have such,
			applies forced Mover to Trainee (which should move Trainee outside), ends when mover is finished
			- 'TrainerFinishSpawn' : plays animation 'Training::Finished' if Trainer have such, 
			otherwise does nothing and ends immediately
			*/
			SpawnInside
		};

	protected:
		/**
		Payment method related function type, changed based on choosen PaymentMethod.
		Returns -1 if resources were successfully checked/taken or lacking resource
		number if there were insufficient resources.
		*/
		typedef bool(SpawnBehaviorFunction)(TrainingComponent* component, Unit* trainer, Unit* trainee);
		/** 

		*/
		std::function<SpawnBehaviorFunction> TrySpawnTrainedUnit;

	public:
		/**
		Determines how much resources are refunded if training is aborted.
		Seperate method may be choosen if training is canceled and if Trainer died.
		*/
		enum RefundMethod
		{
			/**	
			All resources for not fully trained units are refunded
			*/
			FullRefund = 0,	
			/**	
			All resources for queued units are refunded, for currently trained unit
			only part of resources is refunded (equal to (1-trainProgress) * refundPart (customisable))
			*/
			PartialRefund,
			/**	
			All resources for queued units are refunded, for currently trained unit
			no resources are refunded
			*/
			NoRefund,
		};

	public:
		/**	
		Determines in which moment Trainer is able to train next unit,
		when to take resources for new unit and to start actual progress.
		This is controlled by calling 'SetReadyToTrainNext()' and 
		'SetStartTrainingProgress()' by proper states.
		*/
		enum StartTrainMethod
		{
			/**	
			Training can be started after state 'TrainingFinished' begins.
			Resources are taken after state 'TrainingFinished' begins.
			Training progress begins after state 'TrainingFinished' begins.
			*/
			StartTrainingOnFinished = 0,	
			/**	
			Training can be started after state 'TrainingFinished' ends.
			Resources are taken before state 'TrainingPrepare' begins.
			Training progress begins when state 'TrainingPrepare' begins.
			*/
			StartTrainingOnPrepare,
			/**	
			Training can be started after state 'TrainingFinished' ends.
			Resources are taken before state 'TrainingPrepare' begins.
			Training progress begins when state 'Training' begins.
			*/
			StartTrainingOnTraining,
		};

	protected:
		Array<UnitDefinition*> _queue;
		Unit* _currentUnit;
		uint32 _maxSize;

		Event<Unit*, Unit*> _onUnitTrained; // Fired when unit is trained
		Event<Unit*, UnitDefinition*, int> _onQueuedUnitRemoved; // Fired when one of queued unit is removed
		Event<Unit*, UnitDefinition*> _onAddedToQueue;

		bool _paused;
		bool _loopQueue;
		bool _isReadyToTrainNext;
		bool _startTrainingProgress;

		std::function<void(const TrainingComponent&, float ms)> UpdateInternal;
		std::function<void(const TrainingComponent&, float ms)> UpdateInternal_Paused;

	public:
		TrainingComponent(ObjectDefinition* def, IGameObject* owner);
		~TrainingComponent();

		/**
		Updates trained unit progress.
		If no unit is trained / queued or training is paused, does nothing.
		If there's unit trained checks if player have enough resources to progress in training - and
		if he has then updates progress.
		If progress reached 100% then tries to spawn new unit according to choosen SpawnBehavior.
		If spawn was successful then pushes 'TrainingFinished' and fires UnitTrained event.
		If no unit is trained but there is one queued then checks if it can start training - 
		depending on choosen PaymentMethod and StartTrainMethod	states 'TrainingPrepare' and 'Training' are pushed
		Checks for resources are made after 'TrainingFinished' began or ended.

		Depending on state from above list proper Update method is called. 
		*/
		void Update(float ms) { UpdateInternal(*this, ms); }

		void LoadResources(Ogre::SceneManager* ogreMgr);
		void UnloadResources(Ogre::SceneManager* ogreMgr);

		Array<UnitDefinition*>& GetQueuedUnits() { return _queue; }
		const Array<UnitDefinition*>& GetQueuedUnits() const { return _queue; }
		Unit* GetCurrentlyTrainedUnit() const { return _currentUnit; }
		// Returns number of queued units ( not counting currently trained one )
		uint32 GetQueueSize() const { return _queue.size(); }

		bool IsQueueLooped() const { return _loopQueue; }
		void SetQueueLoop(bool value) { _loopQueue = value; }

		uint32 GetMaxSize() const { return _maxSize; }
		void SetMaxSize(uint32 size) { _maxSize = size; }

		bool IsTraining() const
		{
			return _currentUnit != 0; 
		}

		
		bool IsReadyToTrainNext() const { return _isReadyToTrainNext; }
		void SetIsReadyToTrainNext(bool val) { _isReadyToTrainNext = val; }

		bool IsStartTrainingProgress() const { return _startTrainingProgress; }
		void SetIsStartTrainingProgress(bool val) { _startTrainingProgress = val; }

		/**
		Pushes unit to the back of training queue.
		If queue is full then it does nothing and sends notice.
		If player have not enough resources to queue unit (depending on choosen	PaymentMethod)
		then it does nothing and sends notice.
		If successfully added to queue fires 'UnitAddedToQueue' event.
		Does not start training even if thats first unit in queue, but may take resources for it.
		*/
		void AddToQueue(UnitDefinition* definition);

		/**
		Removes first unit of given type from queue (with given definition)
		If 'removeCurrent' == true also currently trained one can be removed
		*/
		void RemoveFirstOfTypeFromQueue(UnitDefinition* definition, bool removeCurrent = false);
		
		/**
		Removes last unit of given type from queue (with given definition)
		If 'removeCurrent' == true also currently trained one can be removed
		*/
		void RemoveLastOfTypeFromQueue(UnitDefinition* definition, bool removeCurrent = false);
		
		/**
		Removes unit from queue at given position and returns resources
		if player paid for it.
		*/
		void RemoveAt(int pos);
		
		/** 
		Removes all units from queue and returns resources if player paid for it. 
		If 'removeCurrent' == true also currently trained one can be removed.
		*/
		void Clear(bool removeCurrent = true);
		
		/** 
		Pauses training of unit if there's one being trained.
		Also no new training will be started.
		*/
		void Pause();
		
		/*
		Resumes training or starts to check for queued units again if component was paused.
		*/
		void Resume();
		
		/** 
		Aborts training of unit if one is being trained and returns resources 
		used for training depending on RefundMethod.
		\param ownerDied if false (default) aborting was due to user's canceling, if true Trainer died
		*/
		void AbortTraining(bool ownerDied = false);

		/** 
		Fired when unit is trained and spawned
		\param owner pointer to Unit owning this component
		\param definition pointer to trained Unit
		*/
		Event<Unit*, Unit*>& UnitTrained() { return _onUnitTrained; }

		/** 
		Fired when one of queued unit is removed.
		\param owner pointer to Unit owning this component
		\param definition pointer to UnitDefinition of removed Unit
		\param position position in queue of removed unit before it was removed or -1 if
		trained unit was removed 
		*/
		Event<Unit*, UnitDefinition*, int>& QueuedUnitRemoved() { return _onQueuedUnitRemoved; }

		/** 
		Fired when unit is successfully added to queue.
		\param owner pointer to Unit owning this component
		\param definition pointer to UnitDefinition of added Unit
		*/
		Event<Unit*, UnitDefinition*>& UnitAddedToQueue() { return _onAddedToQueue; }

		PaymentMethod GetPaymentMethod() const;	
		SpawnBehavior GetSpawnBehavior() const;
		RefundMethod GetRefundMethodOnCancel() const;
		RefundMethod GetRefundMethodOnDeath() const;
		StartTrainMethod GetStartTrainMethod() const;

	protected:
		void SetSpawnBehavior(SpawnBehavior method);

		void Update_Paused(float ms); 
		void Update_IsTraining(float ms);
		void Update_TriesToSpawn(float ms);
		void Update_WaitForNext(float ms);
		void Update_WaitForProgress(float ms);

		int TryTakeResourcesForStartTraining(UnitDefinition* unit);
		int TryTakeResourcesForTick(Unit* unit, float tickPart);
		int TryTakeResourcesForQueue(UnitDefinition* unit);

		void RefundResourcesForQueued(UnitDefinition* unit);
		void RefundResourcesForTraining(UnitDefinition* unit);
	};

	struct TrainableUnit
	{
		UnitDefinition* Definition;
		Array<const char*> RequirementNames;
		int Number;
	};

	class TrainingComponentDefinition : public ObjectDefinition
	{
		// List of unit def names and their numbers ( unit with number x will be bind to Train{x} command )
		// When creating unit, commands for training units form this array will be added
		Array<TrainableUnit> _trainableUnitsDefs;

		TrainingComponent::PaymentMethod _paymentMethod;
		TrainingComponent::SpawnBehavior _spawnBehavior;
		TrainingComponent::RefundMethod _refundMethodOnCancel;
		TrainingComponent::RefundMethod _refundMethodOnDeath;
		TrainingComponent::StartTrainMethod _startTrainMethod;

	public:
		TrainingComponentDefinition(XmlNode* compNode);
		~TrainingComponentDefinition();

		const Array<TrainableUnit>& GetTrainableUnitsArray() const { return _trainableUnitsDefs; }
		Array<TrainableUnit>& GetTrainableUnitsArray() { return _trainableUnitsDefs; }

		TrainingComponent::PaymentMethod GetPaymentMethod() const { return _paymentMethod; }
		void SetPaymentMethod(TrainingComponent::PaymentMethod method) { _paymentMethod = method; }

		TrainingComponent::SpawnBehavior GetSpawnBehavior() const { return _spawnBehavior; }
		void SetSpawnBehavior(TrainingComponent::SpawnBehavior method) { _spawnBehavior = method; }

		TrainingComponent::RefundMethod GetRefundMethodOnCancel() const { return _refundMethodOnCancel; }
		void SetRefundMethodOnCancel(TrainingComponent::RefundMethod method) { _refundMethodOnCancel = method; }
		
		TrainingComponent::RefundMethod GetRefundMethodOnDeath() const { return _refundMethodOnDeath; }
		void SetRefundMethodOnDeath(TrainingComponent::RefundMethod method) { _refundMethodOnDeath = method; }

		TrainingComponent::StartTrainMethod GetStartTrainMethod() const { return _startTrainMethod; }
		void SetStartTrainMethod(TrainingComponent::StartTrainMethod method) { _startTrainMethod = method; }
	};

	inline TrainingComponent::PaymentMethod TrainingComponent::GetPaymentMethod() const 
	{ 
		return GetDefinition<TrainingComponentDefinition>()->GetPaymentMethod(); 
	}

	inline TrainingComponent::SpawnBehavior TrainingComponent::GetSpawnBehavior() const 
	{
		return GetDefinition<TrainingComponentDefinition>()->GetSpawnBehavior(); 
	}

	inline TrainingComponent::RefundMethod TrainingComponent::GetRefundMethodOnCancel() const 
	{ 
		return GetDefinition<TrainingComponentDefinition>()->GetRefundMethodOnCancel(); 
	}

	inline TrainingComponent::RefundMethod TrainingComponent::GetRefundMethodOnDeath() const 
	{ 
		return GetDefinition<TrainingComponentDefinition>()->GetRefundMethodOnDeath(); 
	}

	inline TrainingComponent::StartTrainMethod TrainingComponent::GetStartTrainMethod() const 
	{
		return GetDefinition<TrainingComponentDefinition>()->GetStartTrainMethod(); 
	}
}