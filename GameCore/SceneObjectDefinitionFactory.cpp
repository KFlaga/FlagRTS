#include "PhysicalObjectDefinition.h"
#include "GameCameraDefinition.h"
#include "SceneMarkerDefinition.h"
#include "FXObjectDefinition.h"

namespace FlagRTS
{
	SceneObjectDefinitionFactory::SceneObjectDefinitionFactory() :
		SubClassXmlFactory("SceneObjectDefiniton")
	{
		RegisterFactory("PhysicalObject", xNew0(PhysicalObjectDefinitionFactory));
		RegisterFactory("SceneMarker", xNew0(SceneMarkerDefinitionFactory));
		RegisterFactory("FXObject", xNew0(FXObjectDefinitionFactory));
		RegisterFactory("GameCamera", xNew0(FinalClassXmlFactory<GameCameraDefinition>));
	}
}
