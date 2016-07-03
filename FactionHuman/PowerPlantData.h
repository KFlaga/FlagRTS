#pragma once

#include <ObjectHandle.h>
#include <DataTypes.h>

namespace FlagRTS
{
	struct DllExclusive PowerPlantTypeData
	{
		float TickLength; // Interval in which resources are generated
		PowerPlantTypeData();
	};

	struct DllExclusive PowerPlantObjectData
	{
		int EnergyPerTick; // Energy generated in one tick ( in type data, as it could depened on location or some effects (boosts) )
		PowerPlantObjectData();
	};
}