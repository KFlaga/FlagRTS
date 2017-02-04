#include "ObjectDefinitionsManager.h"

#include "SceneObjectDefinition.h"
#include "CommandButtonDefinition.h"
#include "WeaponDefinition.h"
#include "IEffect.h"
#include "IMover.h"
#include "IMoveStrategy.h"
#include "FXObjectDefinition.h"
#include "ObjectTypeHelper.h"

#include <StringUtils.h>
#include <OgreResourceGroupManager.h>

namespace FlagRTS
{
	ObjectDefinitionsManager::ObjectDefinitionsManager()
	{
		_defFactory = new SubClassXmlFactory("ObjectDefinition");
		_defFactory->RegisterFactory("SceneObject", xNew0(SceneObjectDefinitionFactory));
		_defFactory->RegisterFactory("Weapon", xNew0(FinalClassXmlFactory<WeaponDefinition>));
		_defFactory->RegisterFactory("CommandButton", xNew0(FinalClassXmlFactory<CommandButtonDefinition>));
		_defFactory->RegisterFactory("Effect", xNew0(EffectFactory));
		_defFactory->RegisterFactory("Mover", xNew0(MoverFactory));
		_defFactory->RegisterFactory("MoveStrategy", xNew0(MoveStrategyFactory));
	}

	ObjectDefinitionsManager::~ObjectDefinitionsManager()
	{
		xDelete(_defFactory);
	}

	void ObjectDefinitionsManager::LoadAllObjectsDefinitions(XmlNode* xmlObjectDefs, const string& modName)
	{
		// Load all types of definitons
		XmlNode* defListNode =  xmlObjectDefs->first_node("DefinitionsList");
		while(defListNode != 0)
		{
			// Find type
			string defType = XmlUtility::XmlGetString(defListNode, "type");
			// Process all definitions
			XmlNode* defNode = defListNode->first_node("Definition");
			while(defNode != 0)
			{
				LoadObjectDefintion(defNode, defType, modName);

				defNode = defNode->next_sibling("Definition");
			}

			defListNode = defListNode->next_sibling("DefinitionsList");
		}
	}

	void ObjectDefinitionsManager::LoadingFinished()
	{
		_onAllLoaded.Fire(this);
		_onAllLoaded.DeleteAll();
	}

	void ObjectDefinitionsManager::LoadObjectDefintion(XmlNode* defNode, const string& defType, const string& modName)
	{
		// Find file with object definition
		string defFile = XmlUtility::XmlGetString(defNode, "file");
		auto files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo(modName, defFile);
		if(files->size() == 0)
		{
			CastException_d((string("Definition file not found: ") + defFile).c_str());
			return;
		}
		auto stream = files->begin()->archive->open(defFile);
		// Parse definition file
		RefPtr<XmlDocument> defXml = XmlUtility::XmlDocFromString(stream->getAsString().c_str());
		LoadObjectDefintion(defXml->first_node("ObjectDefinition"));
		stream->close();
	}

	void ObjectDefinitionsManager::LoadObjectDefintion(XmlNode* defNode)
	{
		string familyName = XmlUtility::XmlGetString(defNode, "family");
		IGameObjectFactory<XmlNode*>* factory = _defFactory->GetFactoryOfType(familyName);

		ObjectDefinition* def = static_cast<ObjectDefinition*>(factory->Create(defNode));

		// Override old definition with same name and type name
		const char* typeName = def->GetFinalTypeName().c_str();
		auto defMapIt = _objectDefinitionsNamed.find(typeName);
		if(defMapIt != _objectDefinitionsNamed.end())
		{
			auto oldDefIt = defMapIt->Value.find(def->GetName().c_str());
			if(oldDefIt != defMapIt->Value.end())
			{
				// Such a definition exists already. Delete old one
				ObjectHandle oldHandle = oldDefIt->Value->GetHandle();
				_objectDefinitions.erase(oldHandle.Object);

				ObjectDefinition* oldDef = oldDefIt->Value;
				xDelete(oldDef);
			}
		}
		else
		{
			// Such type does not exist -> add new map
			_objectDefinitionsNamed[CopyChar(typeName)];
		}
		_objectDefinitions.insert(def->GetHandle().Object, def);
		_objectDefinitionsNamed[typeName][CopyString(def->GetName())] = def;
	}

	ObjectDefinition* ObjectDefinitionsManager::GetObjectDefinitionByHandle(ObjectHandle handle)
	{
		if(handle.Type > _objectDefinitions.size())
			CastException_d("Bad defintion handle");
		else
			return _objectDefinitions[handle.Type];
		return 0;
	}

	ObjectDefinition* ObjectDefinitionsManager::GetObjectDefinitionByName(const string& defName,
		const string& defType)
	{
		auto defTypeIt = _objectDefinitionsNamed.find(defType.c_str());
		if(defTypeIt != _objectDefinitionsNamed.end())
		{
			auto defIt = defTypeIt->Value.find(defName.c_str());
			if(defIt != defTypeIt->Value.end())
				return defIt->Value;
		}
		CastException_d("Bad defintion name");
		return 0;
	}
}