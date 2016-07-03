#pragma once

#include "GenericParamterisedEvent.h"

namespace FlagRTS
{
	class MapInitialisedEvent : public ParametrizedEvent_0
	{
	public:
		DEFINE_SIMPLE_EVENTFACTORY(MapInitialisedEvent);
	};
}