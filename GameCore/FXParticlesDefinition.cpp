#include "FXParticlesDefinition.h"
#include "FXParticles.h"

namespace FlagRTS
{

	FXParticlesDefinition::FXParticlesDefinition()
	{
		SetFinalType(GetTypeId<FXParticles>());
		SetFinalTypeName("FXParticles");
	}

	FXParticlesDefinition::FXParticlesDefinition(XmlNode* soDefNode) :
		FXObjectDefinition(soDefNode)
	{
		SetFinalType(GetTypeId<FXParticles>());
		SetFinalTypeName("FXParticles");
	}

}