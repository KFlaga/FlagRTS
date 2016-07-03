#pragma once

#include <Event.h>
#include <Xml.h>

namespace FlagRTS
{
	// Creates SelfActions - that is trigger-less actions, which
	// are just EventHandlers. Should be used only in SelfEvents
	template<typename Source, typename... Args>
	class ISelfActionFactory
	{
	public:
		ISelfActionFactory() { }
		virtual ~ISelfActionFactory() { }

		virtual EventHandler<Args...>* Create(Source actionNode) = 0;
	};
}