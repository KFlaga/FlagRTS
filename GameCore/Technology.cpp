#include "Technology.h"
#include <Exception.h>
#include "DataTypes.h"

namespace FlagRTS
{
	const char* TechBuildingFamilyNames[] =
	{
		"Economy",
		"Military"
	};

	const char* TechAvailabilityNames[] =
	{
		"Available",
		"Inavailable",
		"Hidden"
	};

	TechBuildingFamily ParseBuildingFamily(const char* name)
	{
		for(int i = 0; i < 2; ++i)
		{
			if( strcmp(name, TechBuildingFamilyNames[i]) == 0)
				return (TechBuildingFamily)i;
		}
		CastException_d((string("Bad building family name: ") + string(name)).c_str());
		return (TechBuildingFamily)0;
	}

	TechAvailability ParseTechAvailability(const char* name)
	{
		for(int i = 0; i < 3; ++i)
		{
			if( strcmp(name, TechAvailabilityNames[i]) == 0)
				return (TechAvailability)i;
		}
		CastException_d((string("Bad tech availablity name: ") + string(name)).c_str());
		return (TechAvailability)0;
	}
}