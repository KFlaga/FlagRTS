#include "FXSoundDefinition.h"
#include "FXParticlesDefinition.h"

namespace FlagRTS
{
	FXObjectDefinitionFactory::FXObjectDefinitionFactory() :
		SubClassXmlFactory("FXObjectDefinition")
	{
		RegisterFactory("FXSound", xNew0(FinalClassXmlFactory<FXSoundDefinition>));
		RegisterFactory("FXParticles", xNew0(FinalClassXmlFactory<FXParticlesDefinition>));
	}
}
