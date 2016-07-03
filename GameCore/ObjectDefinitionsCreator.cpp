#include "ObjectDefinitionsCreator.h"
#include <ErrorCodes.h>
#include <OgreResourceGroupManager.h>

#include "SceneObjectDefinitionFactory.h"
#include "CommandButtonDefinition.h"
#include "WeaponDefinition.h"
#include "EffectFactory.h"
#include "MoverFactory.h"
#include "MoveStrategyFactory.h"
#include "FXObjectDefinitionFactory.h"
#include "ObjectTypeHelper.h"
#include <StringUtils.h>

namespace FlagRTS
{

	DEFINE_FINAL_FACTORY(CommandButtonDefinitionFactory, 
		CommandButtonDefinition, 
		"CommandButtonDefinition", 
		XmlNode*);
	
	DEFINE_FINAL_FACTORY(WeaponDefinitionFactory, 
		WeaponDefinition, 
		"WeaponDefinition", 
		XmlNode*);

	ObjectDefinitionsCreator::ObjectDefinitionsCreator()
	{
		AddObjectDefinitionFactory(xNew0(SceneObjectDefinitionFactory), "SceneObject");
		AddObjectDefinitionFactory(xNew0(CommandButtonDefinitionFactory), "CommandButton");
		AddObjectDefinitionFactory(xNew0(WeaponDefinitionFactory), "Weapon");
		AddObjectDefinitionFactory(xNew0(EffectFactory), "Effect");
		AddObjectDefinitionFactory(xNew0(FXObjectDefinitionFactory), "FXObject");
		AddObjectDefinitionFactory(xNew0(MoverFactory), "Mover");
		AddObjectDefinitionFactory(xNew0(MoveStrategyFactory), "MoveStrategy");
	}

	ObjectDefinitionsCreator::~ObjectDefinitionsCreator()
	{
		for(auto factoryIt = _defFactories.begin(), end = _defFactories.end(); 
			factoryIt != end; ++factoryIt)
		{
			xDelete(factoryIt->Value);
		}
		_defFactories.clear();
	}

	ErrorCode ObjectDefinitionsCreator::LoadAllObjectsDefinitions(XmlNode* xmlObjectDefs, const string& modName)
	{
		// Load all types of definitons
		XmlNode* defListNode =  xmlObjectDefs->first_node("DefinitionsList");
		while(defListNode != 0)
		{
			// Find type
			string defType = defListNode->first_attribute("type",4,false)->value();
			// Process all definitions
			XmlNode* defNode = defListNode->first_node("Definition");
			while(defNode != 0)
			{
				LoadObjectDefintion(defNode, defType, modName);

				defNode = defNode->next_sibling("Definition");
			}

			defListNode = defListNode->next_sibling("DefinitionsList");
		}

		return ErrorCodes::Success;
	}

	void ObjectDefinitionsCreator::LoadingFinished()
	{
		_onAllLoaded.Fire();
		_onAllLoaded.DeleteAll();
	}

	void ObjectDefinitionsCreator::LoadObjectDefintion(XmlNode* defNode, const string& defType, const string& modName)
	{
		auto factory = _defFactories.find(defType);
		if(factory != _defFactories.end())
		{
			// Find file with object definition
			string defFile = defNode->first_attribute("file",4,false)->value();
			auto files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo(modName, defFile);
			if(files->size() == 0)
			{
				CastException_d((string("Definition file not found: ") + defFile).c_str());
				return;
			}
			auto stream = files->begin()->archive->open(defFile);
			// Parse definition file
			RefPtr<XmlDocument> defXml = XmlUtility::XmlDocFromString(stream->getAsString().c_str());

			ObjectDefinition* def((ObjectDefinition*)factory->Value->Create(defXml->first_node("ObjectDefinition")));
			stream->close();

			// Override old definition with same name and type name
			const char* typeName = def->GetFinalTypeName().c_str();
			auto defMapIt = _objectDefinitionsNamed.find(typeName);
			if(defMapIt != _objectDefinitionsNamed.end())
			{
				auto oldDefIt = defMapIt->Value.find(def->GetName().c_str());
				if(oldDefIt != defMapIt->Value.end())
				{
					// Such a definition exists already
					ObjectHandle oldHandle = oldDefIt->Value->GetHandle();
					// Swap new defintion with old one and set correct handle
					def->SetHandleTypePart(oldHandle.Type);
					_objectDefinitions[oldHandle.Type] = def;
					oldDefIt->Value = def;
					return;
				}
			}
			else
			{
				// Such type does not exist -> add new map
				_objectDefinitionsNamed[CopyChar(typeName)];
			}
			def->SetHandleTypePart(_objectDefinitions.size());
			_objectDefinitions.push_back(def);
			_objectDefinitionsNamed[typeName][CopyString(def->GetName())] =  def;
		}
		else
		{
			CastException( (string("Bad object definition type: ") + defType).c_str() );
		}
	}

	ObjectDefinition* ObjectDefinitionsCreator::GetObjectDefinitionByHandle(ObjectHandle handle)
	{
		if(handle.Type > _objectDefinitions.size())
			CastException("Bad defintion handle");
		else
			return _objectDefinitions[handle.Type];
	}

	ObjectDefinition* ObjectDefinitionsCreator::GetObjectDefinitionByName(const string& defName,
		const string& defType)
	{
		auto defTypeIt = _objectDefinitionsNamed.find(defType.c_str());
		if(defTypeIt != _objectDefinitionsNamed.end())
		{
			auto defIt = defTypeIt->Value.find(defName.c_str());
			if(defIt != defTypeIt->Value.end())
				return defIt->Value;
		}
		CastException("Bad defintion name");
	}

	ObjectDefinition* ObjectDefinitionsCreator::GetObjectDefinitionByName(const char* defName,
		const char* defType)
	{
		auto defTypeIt = _objectDefinitionsNamed.find(defType);
		if(defTypeIt != _objectDefinitionsNamed.end())
		{
			auto defIt = defTypeIt->Value.find(defName);
			if(defIt != defTypeIt->Value.end())
				return defIt->Value;
		}
		CastException("Bad defintion name");
	}
}