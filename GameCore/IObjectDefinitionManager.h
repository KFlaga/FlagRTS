#pragma once

#include "DataTypes.h"
#include <Xml.h>
#include <Event.h>
#include <ObjectHandle.h>

namespace FlagRTS
{
	class ObjectDefinition;
	class IObjectDefinitionFactory;

	/// Interface for ObjectDefinition loader / holder
	class IObjectDefinitionManager
	{
	public:
		/// Load all object definitions for given Mod
		/**	
			Load all object definitions declared in Definitions file of the Mod.
			Loaded definitions are stored for later use.
			If definition with same name and type already exists it will be overriden.

			\param modObjectDefsNode root node of mod Definitions file (named "ObjectDefinitions")
			\param modName name of corresponding Mod
		*/
		virtual void LoadAllObjectsDefinitions(XmlNode* modObjectDefsNode, const string& modName) = 0;

		
		// Called to indicate all definitions has been loaded
		// Fires OnAllLoaded event
		/// Called to indicate all definitions has been loaded
		/**
			Should be called by after all object definitions from all used Mods have been loaded.
			Fires 'OnAllDefinitionsLoaded' event.
		*/
		virtual void LoadingFinished() = 0;

		/// Loads single object definition
		/**
			Loads single object definition using its declaration.

			\param defNode node of object definition declaration in Definition file (named "Definition")
			\param defType base object type (like SceneObject)
			\param modName name of corresponding Mod.
		*/
		virtual void LoadObjectDefintion(XmlNode* defNode, const string& defType, const string& modName) = 0;
		
		/// Loads single object definition
		/**
			Loads single object definition from its file. 
			
			\param defNode root node of object .def file (named "ObjectDefinition")
		*/
		virtual void LoadObjectDefintion(XmlNode* defNode) = 0;

		
		/// Returns ObjectDefinition with given name and type
		/**
			Returns previously loaded ObjectDefinition with given name and type.
			May be slower than ByHandle* version, but doesn't require handle, which
			can be retrieved only after definition is loaded

			\param defName ObjectDefinition name
			\param defType ObjectDefinition final type name
			\returns pointer to ObjectDefinition if found or 0 otherwise
		*/
		virtual ObjectDefinition* GetObjectDefinitionByName(const string& defName,
			const string& defType) = 0;

		// Returns object definition found in loaded definitons with given handle
		/// Returns ObjectDefinition with given handle
		/**
			Returns previously loaded ObjectDefinition with given handle.
			May be faster then ByName* version, but requires a handle, which
			can be retrieved only after definition is loaded

			\param handle handle to ObjectDefinition
			\returns pointer to ObjectDefinition if found or 0 otherwise
		*/
		virtual ObjectDefinition* GetObjectDefinitionByHandle(ObjectHandle handle) = 0;

		/// Gets event AllDefinitionsLoaded
		/**
			Returns event that occurs when all definitions from all Mods are loaded.
			May be used for ObjectDefinition retrieval if some object needs it
			
			Event fires only once per life of IObjectDefinitionManager and so
			all event handlers are owned by this object and xDeleted after
			event is fired.

			\returns corresponding Event object with 'this' as parameter
		*/
		virtual Event<IObjectDefinitionManager*>& OnAllDefinitionsLoaded();

		/// Returns ObjectDefinition factory with given type name
		/**
			Returns IObjectDefinitionFactory which creates ObjectDefinition of given final-type.

			\param typeName name of object type
			\returns factory of given type or 0 if factory not found
		*/
		virtual IObjectDefinitionFactory* GetFactoryOfType(const string& typeName) = 0;
		
		/// Registers ObjectDefinition factory with given type name
		/**
			Registers IObjectDefinitionFactory which creates ObjectDefinition of given final-type.
			Manager acquires ownership of factory.
			If factory for given type is already specified, overrides it and xDeletes old one.

			\param typeName name of object type
			\param factory of given type
		*/
		virtual void RegisterFactory(const string& typeName, IObjectDefinitionFactory* factory) = 0;
		
		/// Unregisters ObjectDefinition factory with given type name
		/**
			Unregisters IObjectDefinitionFactory which creates ObjectDefinition of given final-type.
			If there's no factory for given type does nothing.

			\param typeName name of object type
		*/
		virtual void UnregisterFactory(const string& typeName) = 0;
	};
}