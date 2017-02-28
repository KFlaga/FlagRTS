#pragma once

#include "UnitStates.h"

namespace FlagRTS
{
	/**
	Plays not looped animation 'Training::Finished' if Trainer have such, 
	otherwise does nothing and ends immediately. During animation cannot be interrupted.
	Depending on StartTrainMethod of owner TrainingComponent sets IsReadyToTrainNext flag.
	Custom training state must inherit from this state and call parent begin()/end()
	for Component to work properly.
	*/
	class TrainingFinishedState : public UnitState
	{
	public:
		TrainingFinishedState(Unit* owner);

		void Begin();
		void End();
		void Interrupt();
		void Resume();
		void Update(float ms);
	};

	/**
	Plays not looped animation 'Training::Prepare' if Trainer have such, 
	otherwise does nothing and ends immediately. During animation cannot be interrupted.
	Depending on StartTrainMethod of owner TrainingComponent sets StartTrainingProgress flag.
	Custom training state must inherit from this state and call parent begin()/end()
	for Component to work properly.
	*/
	class TrainingPrepareState : public UnitState
	{
	public:
		TrainingPrepareState(Unit* owner);

		void Begin();
		void End();
		void Interrupt();
		void Resume();
		void Update(float ms);
	};

	/**
	Plays looped animation 'Training::Training' if Trainer have such, 
	otherwise does nothing and ends immediately. Runs non-critically.
	Depending on StartTrainMethod of owner TrainingComponent sets StartTrainingProgress flag.
	Custom training state must inherit from this state and call parent begin()/end()
	for Component to work properly.
	*/
	class TrainingState : public UnitState
	{
	public:
		TrainingState(Unit* owner);

		void Begin();
		void End();
		void Interrupt();
		void Resume();
		void Update(float ms);
	};
}