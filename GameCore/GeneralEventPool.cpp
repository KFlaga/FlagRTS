#include "GeneralEventPool.h"
#include "Trigger.h"
#include "GeneralEvents.h"

namespace FlagRTS
{
	GeneralEventPool::GeneralEventPool() :
		IEventPool("General")
	{
		_availableEvents["Initialisation"] = xNew0(MapInitialisedEvent);
	}

	GeneralEventPool::~GeneralEventPool()
	{
		for(auto eIt = _availableEvents.begin(); eIt != _availableEvents.end(); ++eIt)
		{
			xDelete(eIt->second);
		}
	}

	IEvent* GeneralEventPool::FindEvent(const string& typeName)
	{
		auto eIt = _availableEvents.find(typeName);
		if( eIt != _availableEvents.end() )
		{
			return eIt->second;
		}
		return 0;
	}

	void GeneralEventPool::Update(float ms)
	{

	}

	void GeneralEventPool::MapInitialised()
	{
		MapInitialisedEvent* initEvent = static_cast<MapInitialisedEvent*>(_availableEvents["Initialisation"]);
		initEvent->Fire();
	}
}