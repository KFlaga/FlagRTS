#include "TechnologyController.h"
#include <UnitDefinition.h>
#include <GameWorld.h>
#include <MainGameObjectPool.h>
#include <ConstructionManager.h>
#include <TechRequirement.h>
#include <TechnologyManager.h>

//#include <Thread.h>
//#include <ThreadManager.h>
//#include <mutex>

namespace FlagRTS
{
	/*class RequirementsTracker
	{
	TechnologyController* _techCtrl;;
	std::mutex _mutex;
	bool _endWork;

	public:
	RequirementsTracker(TechnologyController* techCtrl) :
	_techCtrl(techCtrl),
	_mutex(),
	_endWork(false)
	{ }

	void Update();

	void Lock() { _mutex.lock(); }
	void Unlock() { _mutex.unlock(); }
	void EndProcessing() { _endWork = true; }
	bool ClosePending() const { return _endWork; }
	};*/

	//void TrackRequirements(RequirementsTracker* tracker);

	TechnologyController::TechnologyController(GlobalTechnologyManager* techMgr, uint8 player) :
		_techsMgr(techMgr),
		_cplayer(player)
		//,_reqsTracker(0)
	{

	}

	TechnologyController::~TechnologyController()
	{

	}

	void TechnologyController::Update(float ms)
	{
		
	}

	void TechnologyController::CheckAllRequirements()
	{
		for(unsigned int i = 0; i < _techsMgr->GetPlayerTechsCount(); ++i)
		{
			auto techMgr = _techsMgr->GetPlayerTechs(i);
			techMgr->CheckAllRequirements();
		}
	}

	void TechnologyController::GetNotHiddenBuildings(uint8 player, Array<TechBuilding*>& targetArray)
	{
		auto techMgr = _techsMgr->GetPlayerTechs(player);
		auto& buildings = techMgr->GetAllBuilidings();
		targetArray.clear();
		targetArray.reserve((int)(buildings.size() * 0.5f));
		for(unsigned int b = 0; b < buildings.size(); ++b)
		{
			if(buildings[b]->Availability != TechAvailability::Hidden)
			{
				targetArray.push_back(buildings[b]);
			}
		}
	}

	Event<TechBuilding*, uint8>& TechnologyController::BuilidngAvailabilityChanged(uint8 player)
	{
		return _techsMgr->GetPlayerTechs(player)->BuilidngAvailabilityChanged();
	}

	//void TechnologyController::BeginRequirementTracking()
	//{
	//	_reqsTracker = New<RequirementsTracker>( this);

	//	Thread* trackerThread = ThreadManager::Instance()->CreateThread("RequirementTrackerThread");
	//	if(!Thread::IsMainThread())
	//	{
	//		Thread* currThread = ThreadManager::Instance()->GetCurrentThread();
	//		currThread->AddChildThread(trackerThread);
	//	}
	//	_reqsTrackerThreadHandle = trackerThread->GetHandle();
	//	trackerThread->Run(TrackRequirements, _reqsTracker);
	//}

	//void TechnologyController::EndRequirementTracking()
	//{
	//	if( _reqsTracker != 0 )
	//	{
	//		_reqsTracker->EndProcessing();
	//		ThreadManager::Instance()->RemoveThread("RequirementTrackerThread");
	//		// xDelete(_reqsTracker);
	//		_reqsTracker = 0;
	//	}
	//}

	/*void TrackRequirements(RequirementsTracker* tracker)
	{
		while(!tracker->ClosePending())
		{
			tracker->Update();
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}
		xDelete(tracker);
	}

	void RequirementsTracker::Update()
	{
		Lock();



		Unlock();
	}*/
}