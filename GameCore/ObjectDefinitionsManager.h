#pragma once

#include "ObjectDefinitionFactory.h"
#include <Exception.h>
#include <HashMap.h>
#include <ArrayMap.h>

#include "IObjectDefinitionManager.h"

namespace FlagRTS
{
	class MainXmlFactory;
	
	/// Default implementation of IObjectDefinitionManager 
	class ObjectDefinitionsManager : public IObjectDefinitionManager
	{
		DISALLOW_COPY(ObjectDefinitionsManager);

	protected:
		// Map of all objects definitions, sorted first by object-part of handle
		typedef HashedMap<size_t, ObjectDefinition*> DefinitonsMap;
		// Map of all objects definitions, sorted first by type name, then definition name
		typedef HashedMaps<ObjectDefinition*>::KeyCCString NamedKindsMap;
		typedef HashedMaps<NamedKindsMap>::KeyCCString NamedDefFamiliesMap;
		
	protected:
		SubObjectDefinitionFactory* _defFactory;
		DefinitonsMap _objectDefinitions;
		NamedDefFamiliesMap _objectDefinitionsNamed;
		Event<IObjectDefinitionManager*> _onAllLoaded;

	public:
		ObjectDefinitionsManager();
		~ObjectDefinitionsManager();

		void LoadAllObjectsDefinitions(XmlNode* modXmlObjectDefs, const string& modName);
		void LoadingFinished();

		void LoadObjectDefintion(XmlNode* defNode, const string& defType, const string& modName);
		void LoadObjectDefintion(XmlNode* defNode);
		
		ObjectDefinition* GetObjectDefinitionByName(const string& defName,
			const string& defType);
		
		ObjectDefinition* GetObjectDefinitionByHandle(ObjectHandle handle);

		IObjectDefinitionFactory* GetFactoryOfType(const string& typeName);
		
		void RegisterFactory(const string& typeName, IObjectDefinitionFactory* factory);
		
		void UnregisterFactory(const string& typeName);
	
		MainXmlFactory* GetObjectDefinitionFactory();

		Event<IObjectDefinitionManager*>& OnAllDefinitionsLoaded() { return _onAllLoaded; }

		DefinitonsMap* GetAllDefinitions() { return &_objectDefinitions; }
	};
}