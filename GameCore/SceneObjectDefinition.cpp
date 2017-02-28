#include "SceneObjectDefinition.h"
#include <Xml.h>
#include <Exception.h>

#include "IObjectSpecificDataSupplier.h"
#include "IKindSpecificDataSupplier.h"
#include <StringUtils.h>
#include "GameWorld.h"
#include "IObjectDefinitionManager.h"
#include "Minimap.h"
#include <OgreMaterialManager.h>

namespace FlagRTS
{
	SceneObjectDefinition::SceneObjectDefinition() :
		_isIndependent(true), 
		_inhertisScale(false)
	{

	}

	SceneObjectDefinition::SceneObjectDefinition(XmlNode* node) :
		ObjectDefinition(node),
		_isIndependent(true), 
		_inhertisScale(false)
	{
		// Read properties from xml ( node should be root node ("ObjectDefinition") )

		// Common flags
		XmlNode* flagsNode = node->first_node("Flags");
		if(flagsNode != 0)
			ParseSOFlags(flagsNode);

		// Common properties
		XmlNode* propsNode = node->first_node("Properties");
		if(propsNode != 0)
			ParseSOProperties(propsNode);

		// SceneObject classes
		XmlNode* classesNode = node->first_node("ObjectClasses");
		if(classesNode != 0)
			ParseSOClasses(classesNode);

		XmlNode* childrenNode = node->first_node("ChildObjects");
		if(childrenNode != 0)
			ParseSOChildObjects(childrenNode);
	}

	SceneObjectDefinition::~SceneObjectDefinition()
	{
		auto& onSpawnHandlers = _onSpawn.GetHandlers();
		for(auto it = onSpawnHandlers.cbegin(); it != onSpawnHandlers.cend(); ++it)
		{
			xDelete(it->Value);
		}
		auto& onDespawnHandlers = _onDespawn.GetHandlers();
		for(auto it = onDespawnHandlers.cbegin(); it != onDespawnHandlers.cend(); ++it)
		{
			xDelete(it->Value);
		}
		for(auto it = _objectClasses.begin(); it != _objectClasses.end(); ++it)
		{
			sDelete(*it);
		}
	}

	void SceneObjectDefinition::ParseSOFlags(XmlNode* flagsNode)
	{
		_isIndependent = XmlUtility::XmlGetFlag(flagsNode, "IsIndependent", true);
		_inhertisScale = XmlUtility::XmlGetFlag(flagsNode, "InheritsScale", false);
	}

	void SceneObjectDefinition::ParseSOProperties(XmlNode* propsNode)
	{

	}

	void SceneObjectDefinition::ParseSOClasses(XmlNode* classesNode)
	{
		XmlNode* classNode = classesNode->first_node("Class",5);
		while(classNode != 0)
		{
			_objectClasses.push_back(CopyChar(XmlUtility::XmlGetString(classNode, "name", 4 )));

			classNode = classNode->next_sibling();
		}
	}

	void SceneObjectDefinition::ParseSOChildObjects(XmlNode* childrenNode)
	{
		XmlNode* objectNode = childrenNode->first_node("SceneObject");
		int children = 0;
		while(objectNode != 0)
		{
			string defName = XmlUtility::XmlGetString(objectNode, "definition");
			string defType = XmlUtility::XmlGetString(objectNode, "type");
			Vector3 pos = XmlUtility::XmlGetXYZ(objectNode->first_node("Position"));
			Quaternion rot = XmlUtility::XmlGetQuaternion(objectNode->first_node("Orientation"));
			++children;

			auto getDefinition = [this, defName, defType, pos, rot](IObjectDefinitionManager* mgr) 
			{
				SceneObjectDefinition* soDef = static_cast<SceneObjectDefinition*>(
					mgr->GetObjectDefinitionByName(defType, defName));
				_childObjects.push_back(std::make_pair(soDef, std::make_pair(pos, rot)));
			};
			typedef DelegateEventHandler<decltype(getDefinition), IObjectDefinitionManager*> DefinitionsLoadedHandler;
			GameInterfaces::GetObjectDefinitionManager()->OnAllDefinitionsLoaded() +=
				xNew1(DefinitionsLoadedHandler, getDefinition);

			objectNode = objectNode->next_sibling();
		}

		_childObjects.reserve(children);
	}
}