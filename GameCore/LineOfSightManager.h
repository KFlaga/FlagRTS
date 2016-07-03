#pragma once

#include "DataTypes.h"
#include <Array.h>

namespace PathFinding
{
	template<typename VectorT>
	class IUnitUnitPairer;
}

namespace FlagRTS
{
	class PathingSystem;
	class Dbvh;
	class PathUnit;

	struct UnitLOS
	{
		PathUnit* Unit;
		Array<PathUnit*> UnitsInLOS;
	};

	// Tracks all units line of sight, above all units that each unit can see
	// Units line of sight is defined by its LookupShape
	// Units a unit can see are searched for in PathingSystem Dbvh
	// LOS may be used for :
	// - unit collision predicting and avoidance ( smart moving )
	// - engaging / fleeing from enemy units
	// - 
	class LineOfSightManager
	{
	private:
		Array<UnitLOS> _unitsLOS; // Array of unit LOSes -> for each unit an array of seen units
		PathFinding::IUnitUnitPairer<Vector2>* _uuPairer; // Pairs units

	public:
		LineOfSightManager();

		void Update(float ms);

	private:
		void RefreshUnitsLOS();
	};
}