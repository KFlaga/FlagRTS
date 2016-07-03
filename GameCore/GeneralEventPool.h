#pragma once

#include "IEventPool.h"
#include <map>

namespace FlagRTS
{
	class GeneralEventPool : public IEventPool
	{
	private:
		std::map<string, IEvent*> _availableEvents;

	public:
		GeneralEventPool();
		~GeneralEventPool();

		IEvent* FindEvent(const string& typeName);

		void Update(float ms);

		// GeneralEventPool is special one, as it holds Initialisation, which will be
		// fired when this is called from MainEventPool
		void MapInitialised();
	};
}