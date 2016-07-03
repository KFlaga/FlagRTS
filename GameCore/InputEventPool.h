#pragma once

#include "IEventPool.h"
#include <map>

namespace FlagRTS
{
	class InputEventPool : public IEventPool
	{
	private:
		std::map<string, IEvent*> _availableEvents;

	public:
		InputEventPool();
		~InputEventPool();

		IEvent* FindEvent(const string& typeName);

		void Update(float ms);
	};
}