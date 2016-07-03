#include "PathRequestHandler.h"
#include <PathFindingIPathFinder.h>
#include <Thread.h>
#include <ThreadManager.h>
#include "PathingPathUnit.h"
#include <Profiler.h>

namespace FlagRTS
{
	using namespace PathFinding;

	void PathRequestHandler::AddRequest(const IntVector2& begin, const IntVector2& end, PathUnit* unit)
	{
		_reqMutex.lock();
		_requests.push_back(PathRequest(begin, end, unit));
		if(_firstPendingRequest == _requests.end())
		{
			_firstPendingRequest = --_requests.end();
		}
		_reqMutex.unlock();
	}

	void PathRequestHandler::RemoveRequest(PathUnit* unit)
	{
		_reqMutex.lock();
		for(auto reqIt = _requests.begin(); reqIt != _requests.end(); ++reqIt)
		{
			if(reqIt->Value.Unit == unit)
			{
				InvalidateRequest(reqIt);
				break;
			}
		}
		_reqMutex.unlock();
	}

	PathRequest* PathRequestHandler::GetFirstFinishedRequest()
	{
		PathRequest* req = 0;
		//_reqMutex.lock();
		auto reqIt = _requests.begin();
		for(; reqIt != _requests.end(); ++reqIt)
		{
			if(reqIt->Value.IsFinished == false)
				break;
			if(reqIt->Value.IsValid)
			{
				req = &reqIt->Value;
				break;
			}
			else
				_requests.pop_front();
		}
		//_reqMutex.unlock();
		return req;
	}

	void PathRequestHandler::ProcessNextRequest()
	{
		try
		{
			PROFILE_REGISTER_CLOCK(maskClock, "PathRequestHandler::ProcessNextRequest");
			PROFILE_START(maskClock);
			PathRequest* req = &(_firstPendingRequest->Value);
			_reqMutex.lock();
			if(req->IsValid == false)
			{
				auto it = _firstPendingRequest;
				++_firstPendingRequest;
				_requests.erase(it);
				_reqMutex.unlock();
				return;
			}
			req->IsBeingProcessed = true;
			_reqMutex.unlock();

			// Request new global path
			_pathFinder->SetBlockFilter(req->Unit->Unit->GetCollsionFilter());
			_pathFinder->SetUnitSize(req->Unit->CellSize);
			_pathFinder->FindPath(req->Begin, req->End);

			_reqMutex.lock();
			req->IsBeingProcessed = false;
			req->IsFinished = true;
			if(req->IsValid == false)
			{
				_firstPendingRequest = _requests.erase(_firstPendingRequest);
			}
			else
			{
				++_firstPendingRequest;

				auto& path = _pathFinder->GetLastPathReal();
				req->Unit->Path.clear();
				if(path.size() > 0)
				{
					req->Unit->Path.reserve(path.size());
					for(int i = path.size() - 1; i >= 0; --i)
						req->Unit->Path.push_back(path[i]);
				}
			}
			_reqMutex.unlock();
			PROFILE_END(maskClock);
		}
		catch(...)
		{
			// Something went wrong -> reset first pending request
			_ASSERT(false);
			if(_firstPendingRequest != _requests.end())
				_firstPendingRequest->Value.IsValid = false;

			auto reqIt = _requests.begin();
			for(; reqIt != _requests.end(); ++reqIt)
			{
				if(reqIt->Value.IsFinished == false)
				{
					_firstPendingRequest = reqIt;
					break;
				}
			}
			_reqMutex.unlock();
		}
	}

	void ProcessGlobalPathRequests(PathRequestHandler* requestHandler)
	{
		while(!requestHandler->ClosePending())
		{
			while(requestHandler->RequestsPending())
			{
				requestHandler->ProcessNextRequest();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		xDelete(requestHandler);
	}

	inline void PathRequestHandler::InvalidateRequest(List<PathRequest>::iterator& reqIt)
	{
		// MUTEX SHOULD BE LOCKED
		reqIt->Value.IsValid = false;
		if(reqIt->Value.IsFinished == true)
		{
			// We can erase it ( it should not being used )
			_requests.erase(reqIt);
		}
		else if(reqIt->Value.IsBeingProcessed == true)
		{
			// Need to wait for processing to end to not invalidate iterator ( so do nothing now )
		}
		else if( reqIt == _firstPendingRequest )
		{
			// This id first pending, but its not processed yet, so it safe to remove it
			// Move pending to next
			_firstPendingRequest == _requests.erase(reqIt);
		}
		else
		{
			// It is somewhere between first pending and last request - safe to just remove
			_requests.erase(reqIt);
		}
	}
}