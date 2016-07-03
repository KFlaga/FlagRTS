#pragma once

#include <DataTypes.h>

namespace FlagRTS
{
	class Unit;
	class PhysicalObject;

	struct DllExclusive LogisticsCenterTypeData
	{
		float BaseInfluenceRadius;

		LogisticsCenterTypeData();
	};

	struct DllExclusive LogisticsCenterObjectData
	{
		// For sure build extensions info
		float CurrentInfluenceRadius;
		float CurrentSquaredInfluenceRadius;

		LogisticsCenterObjectData();
	};
}