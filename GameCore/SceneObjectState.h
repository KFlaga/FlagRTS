#pragma once

#include <TypeDefs.h>

namespace FlagRTS
{
	namespace SceneObjectStates
	{
		enum StateType : size_t
		{
			// Generic
			Unknown = 0,
			NotSpawned,
			OnSpawn,
			OnDespawn,
			Idle,
			Move,
			Other,
			Internal,
			Open,
			Close,

			// Unit
			Attack,
			AttackMove,
			Dying,
			ForceMove,
			UnderConstruction,
			Research,

			// FX
			Playing,
			Stopped,
			Paused,

			// TrainingComponent
			Training,
			TrainingStart,
			TrainingPrepare,
			TrainingFinished,

			StateTypesCount,
		};

		StateType ParseStateType(const char* name);
	}
	typedef SceneObjectStates::StateType SceneObjectStateType;

	namespace StateStatuses
	{
		enum StateStatus : int
		{
			Unknown = 0,
			Preparation = 1, // It is pushed but waits for some condition before it can start (OPTIONAL)
			Ready = 2, // Ready to be started
			RunningCritical = 4, // State is running normally and cannot be interrupted
			RunningNoncritical = 8, // State is running normally and can be interrupted
			Paused = 16, // Interrupted state may change to "paused" - then it is resumed - or "ready" - it is started again
			Finished = 32, // State finished bound action and is ready to be popped - and should be popped.
		};

		inline bool IsRunning(StateStatus status)
		{
			return (status & (RunningCritical | RunningNoncritical)) != 0;
		}
	}
	typedef StateStatuses::StateStatus StateStatus;

	/**	
	TODO: change SOStates so that they comply with below notes

	State of SceneObject represents its present physical/logical state.
	Defines things like:
	- animation
	- object movement to supplied target (Using Mover or with one-frame distance target updated every frame externally)
	- ability to handle interaction (selection etc)
	- doing some simple thing that begins with state and ends with state 

	Should not include:
	- chain of actions 
	- making decisions what to do next after state finished
	- executing commands (for Units)
	- 

	State can have some states itself (status):
	- "preparation" - when it is pushed but waits for some condition before it can start (OPTIONAL)
	- "ready" - ready to be started
	- "running critical" - state is running normally and cannot be interrupted
	- "running non-critical" - state is running normally and can be interrupted
	- "paused" - when interrupted state may change to "paused" - then it is resumed - or "ready" - it is started again
	- "finished" - state finished bound action and is ready to be popped - and should be popped.

	If state does finished work or does nothing but object should remain in this state anyway 
	it should be in "running non-critical".
	*/
	class SceneObjectState
	{
	protected:
		StateStatus _status;
		size_t _type;
		const char* _name;

	public:
		SceneObjectState(size_t type, const char* name) :
			_status(StateStatus::Ready),
			_type(type),
			_name(name)
		{ }

		virtual ~SceneObjectState()
		{

		}

		/** 
		Called when state is started first time.
		May end with any status.
		*/
		virtual void Begin() 
		{ 
			_status = StateStatus::RunningNoncritical;
		}

		/** 
		Called when state is to be popped.
		Should end with "Ready" or "Preparation" status.
		*/
		virtual void End() 
		{ 
			_status = StateStatus::Ready;
		}

		/** 
		Called each frame.
		After state finished working should end with "Finished" status.
		*/
		virtual void Update(float ms) = 0;

		/** 
		Called when new state is pushed but this one is not finished 
		Should end with "Paused", "Finished", "Ready" or "Preparation" status.
		*/
		virtual void Interrupt() 
		{ 
			_status = StateStatus::Paused;
		}

		/**
		Called when states on top of it finished work after interruption if
		current state is "paused".
		May end with any status.
		*/
		virtual void Resume() 
		{ 
			_status = StateStatus::RunningNoncritical;
		}

		/** 
		Returns name of state (for parsing) 
		*/
		const char* GetName() const
		{
			return _name;
		}

		/** 
		Returns TypeId of state
		*/
		size_t GetType() const
		{
			return SceneObjectStates::Unknown;
		}
		
		/**
		Updates and returns current status
		*/
		virtual StateStatus CheckStatus() const
		{
			_ASSERT(_status != StateStatus::Unknown);
			return _status;
		}
	};
}