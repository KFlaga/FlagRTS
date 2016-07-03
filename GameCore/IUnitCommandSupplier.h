#pragma once

#include <TypeDefs.h>

namespace FlagRTS
{
	class Unit;

	// Supplies unit with command based on its class
	// 
	class IUnitCommandSupplier
	{
	public:
		virtual void AddCommandsToUnit(Unit* unit) = 0;
	};
}