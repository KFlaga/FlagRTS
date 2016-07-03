#pragma once

#include <Windows.h>
#include "TypeDefs.h"

namespace FlagRTS
{
	// Clock is used to get time passed ( unlike timers, which fires on time elapsed )
	//
	class Clock
	{
	private:
		LARGE_INTEGER _clockFreq;
		LARGE_INTEGER _ticksOnStart;
		LARGE_INTEGER _ticksOnPause;
		LARGE_INTEGER _ticksOnLastCheck;
		LARGE_INTEGER _currentTicks;

		bool _isRunning;

	public:
		Clock() 
		{
			QueryPerformanceFrequency(&_clockFreq);
		}

		~Clock()
		{
			
		}

		void Start()
		{
			_isRunning = true;
			Reset();
		}

		void Resume()
		{
			_isRunning = true;
		}

		void Pause()
		{
			QueryPerformanceCounter(&_ticksOnPause);
			_isRunning = false;
		}

		// Reset start time, do not stops/starts clock
		void Reset()
		{
			QueryPerformanceCounter(&_ticksOnStart);
			QueryPerformanceCounter(&_ticksOnPause);
			QueryPerformanceCounter(&_ticksOnLastCheck);
		}

		// return ms/secs since last call to start or reset
		// on pause time when paused
		// Secs are rounded to nearest
		int64 GetMilisecSinceStart()
		{
			if(_isRunning)
			{
				QueryPerformanceCounter(&_currentTicks);
				return GetDiffTimeMs(_ticksOnStart, _currentTicks);
			}
			else
			{
				return GetDiffTimeMs(_ticksOnStart, _ticksOnPause);
			}
		}

		float GetMilisecSinceStartR()
		{
			if(_isRunning)
			{
				QueryPerformanceCounter(&_currentTicks);
				return GetDiffTimeMsR(_ticksOnStart, _currentTicks);
			}
			else
			{
				return GetDiffTimeMsR(_ticksOnStart, _ticksOnPause);
			}
		}

		int64 GetSecSinceStart()
		{
			if(_isRunning)
			{
				QueryPerformanceCounter(&_currentTicks);
				return GetDiffTimeSec(_ticksOnStart, _currentTicks);
			}
			else
			{
				return GetDiffTimeSec(_ticksOnStart, _ticksOnPause);
			}
		}

		float GetSecSinceStartR()
		{
			if(_isRunning)
			{
				QueryPerformanceCounter(&_currentTicks);
				return GetDiffTimeSecR(_ticksOnStart, _currentTicks);
			}
			else
			{
				return GetDiffTimeSecR(_ticksOnStart, _ticksOnPause);
			}
		}

		// returns time since last call to this function
		// or start/reset if function was not called after last start/reset
		//  on pause time when paused
		int64 GetMilisecSinceLastCheck()
		{
			int64 time;
			if(_isRunning)
			{
				QueryPerformanceCounter(&_currentTicks);
				time = GetDiffTimeMs(_ticksOnLastCheck, _currentTicks);
				_ticksOnLastCheck = _currentTicks;
			}
			else
			{
				time = GetDiffTimeMs(_ticksOnLastCheck, _ticksOnPause);
				_ticksOnLastCheck = _currentTicks;
			}
			return time;
		}

		float GetMilisecSinceLastCheckR()
		{
			float time;
			if(_isRunning)
			{
				QueryPerformanceCounter(&_currentTicks);
				time = GetDiffTimeMsR(_ticksOnLastCheck, _currentTicks);
				_ticksOnLastCheck = _currentTicks;
			}
			else
			{
				time = GetDiffTimeMsR(_ticksOnLastCheck, _ticksOnPause);
				_ticksOnLastCheck = _currentTicks;
			}
			return time;
		}

		int64 GetSecSinceLastCheck()
		{
			int64 time;
			if(_isRunning)
			{
				QueryPerformanceCounter(&_currentTicks);
				time = GetDiffTimeSec(_ticksOnLastCheck, _currentTicks);
				_ticksOnLastCheck = _currentTicks;
			}
			else
			{
				time = GetDiffTimeSec(_ticksOnLastCheck, _ticksOnPause);
				_ticksOnLastCheck = _currentTicks;
			}
			return time;
		}

		float GetSecSinceLastCheckR()
		{
			float time;
			if(_isRunning)
			{
				QueryPerformanceCounter(&_currentTicks);
				time = GetDiffTimeSecR(_ticksOnLastCheck, _currentTicks);
				_ticksOnLastCheck = _currentTicks;
			}
			else
			{
				time = GetDiffTimeSecR(_ticksOnLastCheck, _ticksOnPause);
				_ticksOnLastCheck = _currentTicks;
			}
			return time;
		}
		
	private:
		int64 GetDiffTimeMs(LARGE_INTEGER t1, LARGE_INTEGER t2)
		{
			LARGE_INTEGER diff;
			diff.QuadPart = t2.QuadPart - t1.QuadPart;
			diff.QuadPart *= 1000;
			diff.QuadPart /= _clockFreq.QuadPart;
			return diff.QuadPart;
		}

		int64 GetDiffTimeSec(LARGE_INTEGER t1, LARGE_INTEGER t2)
		{
			LARGE_INTEGER diff;
			diff.QuadPart = t2.QuadPart - t1.QuadPart;
			diff.QuadPart /= _clockFreq.QuadPart;
			return diff.QuadPart;
		}

		float GetDiffTimeMsR(LARGE_INTEGER t1, LARGE_INTEGER t2)
		{
			float diff;
			diff = (t2.QuadPart - t1.QuadPart)*1000;
			diff /= (float)_clockFreq.QuadPart;
			return diff;
		}

		float GetDiffTimeSecR(LARGE_INTEGER t1, LARGE_INTEGER t2)
		{
			float diff;
			diff = (t2.QuadPart - t1.QuadPart);
			diff /= (float)_clockFreq.QuadPart;
			return diff;
		}
	};
}