#pragma once

#include <IFactory.h>
#include <IGameObject.h>
#include "DataTypes.h"
#include <Xml.h>
#include <unordered_map>
#include <TypeId.h>
#include <ObjectHandle.h>
#include <Exception.h>
#include "ObjectDefinition.h"
#include <Event.h>
#include <HashMap.h>
#include <ArrayMap.h>

namespace FlagRTS
{

	// MapCreator is reponsible for loading all definitions of 
	// of objects which will be placed on map and creating indivdual
	// objects to be attached to current map
	// It does not however stores created objects instances themselves
	// (only type definitions) and does not attach them ( it is responsibility
	// of GameWorld class )
	class ObjectDefinitionsCreator
	{
		// Use string as key, as factory type will be read from file, that is from string
		typedef ArrayMaps<IFactory<XmlNode*>*>::KeyString DefFactoryMap;
		// Map of all objects definitions, sorted first by type-part of handle ( which is just index in this table )
		typedef Array<ObjectDefinition*> DefinitonsMap;
		// Map of all objects definitions, sorted first by type name, then definition name
		typedef HashedMaps<ObjectDefinition*>::KeyCCString NamedKindsMap;
		typedef HashedMaps<NamedKindsMap>::KeyCCString NamedDefFamiliesMap;
		
	private:
		DefFactoryMap _defFactories;
		DefinitonsMap _objectDefinitions;
		NamedDefFamiliesMap _objectDefinitionsNamed;
		Event<> _onAllLoaded;

	public:
		ObjectDefinitionsCreator();
		~ObjectDefinitionsCreator();

		// Loads all object defintions from given node and adds them to current definitions
		// If definition of object with same name exist it will be overriden
		ErrorCode LoadAllObjectsDefinitions(XmlNode* modXmlObjectDefs, const string& modName);

		// Called to indicate all definitions has been loaded
		// Fires OnAllLoaded event
		void LoadingFinished();

		void LoadObjectDefintion(XmlNode* defNode, const string& defType, const string& modName);

		// Returns ObjectDefinition of specified type with given name and type
		ObjectDefinition* GetObjectDefinitionByName(const char* defName,
			const char* defType);

		// Returns ObjectDefinition of specified type with given name and type
		ObjectDefinition* GetObjectDefinitionByName(const string& defName,
			const string& defType);

		template<typename ObjectDefinitionType>
		ObjectDefinitionType* GetObjectDefinitionByNameCast(const char* defName,
			const char* defType)
		{
			return static_cast<ObjectDefinitionType*>(
				GetObjectDefinitionByName(defName, defType));
		}

		template<typename ObjectDefinitionType>
		ObjectDefinitionType* GetObjectDefinitionByNameCast(const string& defName,
			const string& defType)
		{
			return static_cast<ObjectDefinitionType*>(
				GetObjectDefinitionByName(defName, defType));
		}

		// Returns object definition found in loaded definitons with given handle
		ObjectDefinition* GetObjectDefinitionByHandle(ObjectHandle handle);

		template<typename ObjectDefinitionType>
		ObjectDefinitionType* GetObjectDefinitionByHandleCast(ObjectHandle handle)
		{
			return return static_cast<ObjectDefinitionType*>(
				OGetObjectDefinitionByHandle(handle));
		}
		
		void AddObjectDefinitionFactory(IFactory<XmlNode*>* factory, const string& typeName) NO_THROW
		{
			_defFactories.insert(std::make_pair(typeName, factory));
		}

		Event<>& OnAllDefinitionsLoaded() { return _onAllLoaded; }

		DefinitonsMap* GetAllDefinitions() { return &_objectDefinitions; }
	};
}