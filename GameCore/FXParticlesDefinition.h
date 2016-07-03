#pragma once

#include "FXObjectDefinition.h"

namespace FlagRTS
{
	// Particle system defines everything in script, so nothing new here
	class FXParticlesDefinition : public FXObjectDefinition
	{
	public:
		FXParticlesDefinition();
		FXParticlesDefinition(XmlNode* soDefNode);
	};
}