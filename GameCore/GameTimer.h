#pragma once

#include <IGameObject.h>
#include <Event.h>
#include <map>

namespace FlagRTS
{
	// Counts time that elapsed in game
	// Differs from Clock in a way, that it is updated with every game
	// tick and can handle elapsed events
	class GameTimer : public IGameObject
	{
	private:
		double _curTime;
		double _targetTime;
		Event<GameTimer*> _onElapsed;
		bool _isRunning;

	public:
		GameTimer() : 
			_curTime(0), _targetTime(0) 
		{ }

		// Starts counting time. Do not reset timer, so used also to resume from pause
		void Start()
		{
			_isRunning = true;
		}
		// Pauses timer, but do not reset it
		void Pause()
		{
			_isRunning = false;
		}
		// Pauses timer and resets it
		void Stop()
		{
			_isRunning = false;
			_curTime = 0.0;
		}

		void Update(float ms)
		{
			_curTime += ms;
			if(_curTime >= _targetTime)
			{
				_onElapsed.Fire(this);
				Stop();
			}
		}

		void SetTargetTime(double ms) { _targetTime = ms; }
		double GetTargetTime() const { return _targetTime; }

		double GetCurrentTime() const { return _curTime; }

		Event<GameTimer*>& TimeElapsed() { return _onElapsed; }
	};

	// Global timer dont have single elapsed event, but can handle
	// many elapsed events with different times
	class GlobalGameTimer
	{

	};
}