#pragma once

#include <Array.h>

namespace FlagRTS
{
	class UnitDefinition;
	class ConstructionPlanner;
	class TechRequirement;
	class TechRequirementFactory;

	enum TechAvailability : int
	{
		Available = 0,
		Inavailable,
		Hidden,

		Unchecked = 200
	};
	TechAvailability ParseTechAvailability(const char* name);

	enum TechBuildingFamily : int
	{
		Economy = 0,
		Military = 1,

		NoFamily = 201
	};
	TechBuildingFamily ParseBuildingFamily(const char* name);

	struct TechBuilding
	{
		UnitDefinition* Building;
		TechAvailability Availability;
		TechBuildingFamily Family;
		ConstructionPlanner* UsedPlanner;

		Array<TechRequirement*> Requirements;

		TechBuilding() :
			Building(0),
			Availability(Unchecked),
			Family(NoFamily),
			UsedPlanner(0)
		{ }
	};

	struct TechUnit
	{
		UnitDefinition* Unit;
		TechAvailability Availability;
		Array<TechRequirement*> Requirements;

		TechUnit() :
			Unit(0),
			Availability(Unchecked)
		{ }
	};
}