#pragma once

#include <TypeDefs.h>

namespace FlagRTS
{
	// Task is just a function to be called when the time is right
	// Here it is used to split potentialy long operations into multiple
	// shorter ones to ensure synchronous operation and regular updates
	// In subclass just overload operator()
	class Task
	{
	public:
		virtual void operator()() { }
	};
}