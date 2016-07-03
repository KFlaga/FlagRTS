#pragma once

#include "ConstructionPlanner.h"

namespace FlagRTS
{
	// Changed version of const. planner -> it have addition restrictions on checking 
	// validity of construction, that is only specific points (cells) can be used as builing place
	// ( like extensions of buildings, which must be placed in valid spots, or wall mounted turrets )
	// Funcinality from base ConstructionPlanner is extened by passing additional arguments, so
	// if they are not specified it behaves as standard version, so it may be used for 
	// normal building as well
	class ExtensionPlanner : public ConstructionPlanner
	{

	};
}