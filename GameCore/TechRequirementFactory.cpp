
#include "PlayerUnitKindRequirement.h"
#include <Exception.h>

namespace FlagRTS
{
	TechRequirementFactory::TechRequirementFactory() :
		SubClassXmlFactory("TechRequirement")
	{
		RegisterFactory("HaveAtLeastUnits", xNew0(FinalClassXmlFactory<PlayerHaveAtLeastUnitsOfKindRequirement>));
		RegisterFactory("HaveNoMoreThanUnits", xNew0(FinalClassXmlFactory<PlayerHaveNoMoreThanUnitsOfKindRequirement>));
		RegisterFactory("HaveExaclyUnits", xNew0(FinalClassXmlFactory<PlayerHaveExaclyUnitsOfKindRequirement>));
		RegisterFactory("SpawnedAtLeastUnits", xNew0(FinalClassXmlFactory<PlayerSpawnedAtLeastUnitsOfKindRequirement>));
		RegisterFactory("SpawnedNoMoreThanUnits", xNew0(FinalClassXmlFactory<PlayerSpawnedNoMoreThanUnitsOfKindRequirement>));
		RegisterFactory("SpawnedExaclyUnits", xNew0(FinalClassXmlFactory<PlayerSpawnedExaclyUnitsOfKindRequirement>));
	}
}