#include "TerrainProjectionMarkerDefinition.h"
#include "TerrainProjectionMarker.h"

namespace FlagRTS
{
	TerrainProjectionMarkerDefinition::TerrainProjectionMarkerDefinition() :
		SceneMarkerDefinition()
	{
		SetFinalType(GetTypeId<TerrainProjectionMarker>());
		SetFinalTypeName("TerrainProjectionMarker");
	}

	TerrainProjectionMarkerDefinition::TerrainProjectionMarkerDefinition(XmlNode* soDefNode) :
		SceneMarkerDefinition(soDefNode)
	{
		SetFinalType(GetTypeId<TerrainProjectionMarker>());
		SetFinalTypeName("TerrainProjectionMarker");

		XmlNode* tpmNode = soDefNode->first_node("SceneMarker")->
			first_node("TerrainProjectionMarker");

		_textureName = XmlUtility::XmlGetString(tpmNode->first_node("Texture"), "name");
		_baseHalfSize = XmlUtility::XmlGetXYIfExists(tpmNode->first_node("HalfSize"), Vector2(0.f,0.f));
	}
}