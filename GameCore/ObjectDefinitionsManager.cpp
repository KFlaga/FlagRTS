#include "ObjectDefinitionsManager.h"

#include <StringUtils.h>
#include <OgreResourceGroupManager.h>

#include "UnitDefinition.h"
#include "MapDecoratorDefinition.h"
#include "GameCameraDefinition.h"
#include "MissleDefinition.h"
#include "BillboardMarkerDefinition.h"
#include "ModelMarkerDefinition.h"
#include "TerrainProjectionMarkerDefinition.h"
#include "FXSoundDefinition.h"
#include "FXParticlesDefinition.h"

#include "MinimapComponent.h"
#include "SelectionComponent.h"

namespace FlagRTS
{
	ObjectDefinitionsManager::ObjectDefinitionsManager()
	{
		_defFactory = new SubObjectDefinitionFactory();

		RegisterFactory("GameCamera", xNew0(FinalObjectDefinitionFactory<GameCameraDefinition>));

		// Physical objects
		RegisterFactory("Unit", xNew0(FinalObjectDefinitionFactory<UnitDefinition>));
		RegisterFactory("MapDecorator", xNew0(FinalObjectDefinitionFactory<MapDecoratorDefinition>));
		RegisterFactory("Missle", xNew0(FinalObjectDefinitionFactory<MissleDefinition>));

		// Scene markers
		RegisterFactory("TerrainProjectionMarker", xNew0(FinalObjectDefinitionFactory<TerrainProjectionMarkerDefinition>));
		RegisterFactory("ModelMarker", xNew0(FinalObjectDefinitionFactory<ModelMarkerDefinition>));
		RegisterFactory("BillboardMarker", xNew0(FinalObjectDefinitionFactory<BillboardMarkerDefinition>));

		// Fx objects
		RegisterFactory("FXSound", xNew0(FinalObjectDefinitionFactory<FXSoundDefinition>));
		RegisterFactory("FXParticles", xNew0(FinalObjectDefinitionFactory<FXParticlesDefinition>));

		// Movers
		//RegisterFactory("Line", xNew0(FinalObjectDefinitionFactory<LineMover>));
		//RegisterFactory("AcceleratedLine", xNew0(FinalObjectDefinitionFactory<AcceleratedLineMover>));

		//RegisterFactory("NoMove", xNew0(FinalObjectDefinitionFactory<NoMoveMoveStrategy>));
		//RegisterFactory("NoMove", xNew0(FinalObjectDefinitionFactory<FaceAndGoMoveStrategy>));
		//RegisterFactory("NoMove", xNew0(FinalObjectDefinitionFactory<RotateOnlyStrategy>));

		// Effects
		/*
		RegisterFactory("LaunchMissle", xNew0(FinalObjectDefinitionFactory<LaunchMissleEffect>));
		RegisterFactory("InflictDamage", xNew0(FinalObjectDefinitionFactory<InflictDamageEffect>));
		RegisterFactory("Multi", xNew0(FinalObjectDefinitionFactory<MultiEffect>));
		RegisterFactory("SpawnObject", xNew0(FinalObjectDefinitionFactory<SpawnObjectEffect>));
		*/

		// Components
		RegisterFactory("MinimapComponent", xNew0(FinalObjectDefinitionFactory<MinimapComponentDefinition>));
		RegisterFactory("SelectionComponent", xNew0(FinalObjectDefinitionFactory<SelectionComponentDefinition>));
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
		ObjectDefinition* def = _defFactory->Create(defNode);

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
		auto it = _objectDefinitions.find(handle.Object);

		if(it != _objectDefinitions.end())
		{
			return it->Value;
		}
		CastException_d("Bad defintion handle");
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

	void ObjectDefinitionsManager::RegisterFactory(const string& typeName, IObjectDefinitionFactory* factory)
	{
		_defFactory->RegisterFactory(typeName, factory);
	}

	void ObjectDefinitionsManager::UnregisterFactory(const string& typeName)
	{
		_defFactory->UnregisterFactory(typeName);
	}

	// ======================================= //

	SubObjectDefinitionFactory::SubObjectDefinitionFactory()
	{ }

	SubObjectDefinitionFactory::~SubObjectDefinitionFactory()
	{
		for(auto factoryIt = _subFactories.begin(); factoryIt != _subFactories.end(); factoryIt++)
		{
			xDelete(factoryIt->Value);
		}
		_subFactories.clear();
	}

	void SubObjectDefinitionFactory::RegisterFactory(const string& typeName, IObjectDefinitionFactory* factory)
	{
		auto it = _subFactories.find(typeName.c_str());
		if(it != _subFactories.end())
		{
			xDelete(it->Value);
		}

		_subFactories.insert(typeName.c_str(), factory);
	}

	void SubObjectDefinitionFactory::UnregisterFactory(const string& typeName)
	{
		auto it = _subFactories.find(typeName.c_str());
		if(it != _subFactories.end())
		{
			xDelete(it->Value);
		}
	}

	IObjectDefinitionFactory* SubObjectDefinitionFactory::GetFactoryOfType(const string& typeName)
	{
		auto it = _subFactories.find(typeName.c_str());
		if(it != _subFactories.end())
		{
			return it->Value;
		}
		else
		{
			CastException_d(string("Requested IObjectDefinitionFactory of type [") + typeName + "] does not exist");
		}
		return 0;
	}

	ObjectDefinition* SubObjectDefinitionFactory::Create(XmlNode* objNode)
	{
		ObjectDefinition* definition = 0;
		string name = XmlUtility::XmlGetStringIfExists(objNode, "name");

		try
		{
			string fullType = XmlUtility::XmlGetString(objNode, "type");
			IObjectDefinitionFactory* factory = GetFactoryOfType(fullType);
			definition = factory->Create(objNode);
		}
		catch(Exception* e)
		{
			throw e; // Rethrow our exception (already logged)
		}
		catch(...) // Failed to create object for some
		{
			CastException(string("MainObjectDefinitionFactory::Create: Error during creation of") + 
				"object definition for object named: [" + name + "].");
		}

		return definition;
	}
}