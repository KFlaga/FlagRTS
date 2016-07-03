#pragma once

#include "DataTypes.h"
#include <Array.h>
#include <List.h>
#include <mutex>

namespace PathFinding
{
	class IGlobalPathFinder;
}

namespace FlagRTS
{
	class PathUnit;

	struct PathRequest
	{
		IntVector2 Begin;
		IntVector2 End;
		PathUnit* Unit;
		bool IsFinished;
		bool IsBeingProcessed;
		bool IsValid;

		PathRequest() :
			IsFinished(false),
			IsValid(true),
			IsBeingProcessed(false)
		{ }

		PathRequest(const IntVector2& begin, const IntVector2& end, PathUnit* unit) :
			Begin(begin),
			End(end), 
			IsFinished(false),
			IsValid(true),
			Unit(unit),
			IsBeingProcessed(false)
		{ }
	};

	class PathRequestHandler
	{
		PathFinding::IGlobalPathFinder* _pathFinder;
		List<PathRequest> _requests;
		List<PathRequest>::iterator _firstPendingRequest;
		std::mutex _reqMutex;
		bool _endWork;

	public:
		PathRequestHandler(PathFinding::IGlobalPathFinder* pathFinder) :
			_pathFinder(pathFinder),
			_endWork(false)
		{ 
			_firstPendingRequest = _requests.end();
		}

		void AddRequest(const IntVector2& begin, const IntVector2& end, PathUnit* unit);
		// If returns 0, theres no finished requests
		PathRequest* GetFirstFinishedRequest();

		void RemoveFinishedRequest()
		{
			//_reqMutex.lock();
			_requests.pop_front();
			//_reqMutex.unlock();	
		}

		void ProcessNextRequest();
		void RemoveRequest(PathUnit* unit);

		void EndProcessing()
		{
			_endWork = true;
		}
		bool ClosePending() const { return _endWork; }
		bool RequestsPending()
		{
			_reqMutex.lock();
			bool pending = _firstPendingRequest != _requests.end();
			_reqMutex.unlock();
			return pending;
		}

		void Lock() { _reqMutex.lock(); }
		void Unlock() { _reqMutex.unlock(); }

	private:
		void InvalidateRequest(List<PathRequest>::iterator& req);
	};

	void ProcessGlobalPathRequests(PathRequestHandler* requestHandler);
}