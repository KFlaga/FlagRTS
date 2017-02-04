#include "TerrainProjectionMarkerDefinition.h"
#include "ModelMarkerDefinition.h"
#include "BillboardMarkerDefinition.h"

namespace FlagRTS
{
	SceneMarkerDefinitionFactory::SceneMarkerDefinitionFactory() :
		SubClassXmlFactory("SceneMarkerDefinition")
	{
		RegisterFactory("TerrainProjectionMarker", xNew0(FinalClassXmlFactory<TerrainProjectionMarkerDefinition>));
		RegisterFactory("ModelMarker", xNew0(FinalClassXmlFactory<ModelMarkerDefinition>));
		RegisterFactory("BillboardMarker", xNew0(FinalClassXmlFactory<BillboardMarkerDefinition>));
	}
}
