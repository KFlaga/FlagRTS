#pragma once

#include "IObjectComponent.h"
#include "ObjectDefinition.h"
#include <Event.h>

namespace FlagRTS
{
	class SceneObject;
	class SceneMarkerDefinition;
	class SceneMarker;

	/**
+		Expected layout of node:
		<Component type="MinimapComponent">
		   <Flags>
				<RevealedVisible/>
				<UseCustomIcon/>
				<MinimapSelectable/>
		   </Flags>
		   <Size x="80.f" y="160.f"/> <!-- Size of minimap icon -> whole minimap is 1000x1000 * aspect -->
		   <CustomIcon name="icon.png"/> <!-- Must be provided if flag UseCustomIcon is set -->
		</Component>
	*/
	class InteractionComponentDefinition : public ObjectDefinition
	{
	public:
		InteractionComponentDefinition(XmlNode* compNode);
	};

	/**
		Component allows for handling user interaction with object.
		Supported interaction:
		- mouse hover
		- mouse clicks
		- 
		Supported actions:
		- events
		- show scene marker
		- animation
		- 
	*/
	class InteractionComponent : public IObjectComponent
	{
	public:
		InteractionComponent(ObjectDefinition* def, IGameObject* owner);
	};
}