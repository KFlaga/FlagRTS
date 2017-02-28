#include "ObjectDefinition.h"
#include "GameWorld.h"
#include "IObjectDefinitionManager.h"
#include "ObjectDefinitionFactory.h"

namespace FlagRTS
{
	ObjectDefinition::ObjectDefinition()
	{

	}

	ObjectDefinition::ObjectDefinition(XmlNode* defNode)
	{
		// Name of definition
		SetName(XmlUtility::XmlGetStringIfExists(defNode, "name"));

		// Find all component nodes
		std::vector<XmlNode*> componentNodes = XmlUtility::XmlFindAllNodesWithName(
			defNode, "Component");
		// For each node : find its type, then factory then create it
		for(int i = 0; i < componentNodes.size(); ++i)
		{
			XmlNode* compNode = componentNodes[i];
			const char* typeName = XmlUtility::XmlGetString(compNode, "type");
			IObjectDefinitionFactory* compFactory = GameInterfaces::GetObjectDefinitionManager()->GetFactoryOfType(typeName);

			_ASSERT(compFactory != 0);
			if(compFactory != 0)
			{
				ObjectDefinition* compDef = compFactory->Create(compNode);
				_componentDefinitions.push_back(compDef);
			}
		}
	}

	ObjectDefinition::~ObjectDefinition()
	{
		for(int i = 0; i < _componentDefinitions.size(); ++i)
		{
			xDelete(_componentDefinitions[i]);
		}
	}
}