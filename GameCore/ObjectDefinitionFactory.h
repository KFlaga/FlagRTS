#pragma once

#include <Xml.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	class ObjectDefinition;

	/**
		Interface for factory of ObjectDefinitions using Xml
	 */
	class IObjectDefinitionFactory
	{
	public:
		IObjectDefinitionFactory() { }
		virtual ~IObjectDefinitionFactory() { }

		/**
			Creates ObjectDefinition from supplied node.
		*/
		virtual ObjectDefinition* Create(XmlNode* definitionNode) = 0;
	};
}

namespace FlagRTS
{
	/** Factory class for final types definitions created from Xml files */
	template<typename DefinitionType>
	class FinalObjectDefinitionFactory : public IObjectDefinitionFactory
	{
		ObjectDefinition* Create(XmlNode* objNode) 
		{
			return xNew1(DefinitionType, objNode); 
		}
	};
	
	/// Factory of all object created from Xml definitions
	/**	
		Creates ObjectDefinitions from Xml definitions.
		Creation of final object is delegated to sub types factories.

		Requirements on Definition file layout:
		- Root node is "ObjectDefinition" node  with attributes "name" and "type".
		Type attribute is final type of game object (not definition) (i.e. for UnitDefinition it is Unit)
		Object kind is then fully defined by pair ('name', 'final_type')
	*/
	class SubObjectDefinitionFactory : public IObjectDefinitionFactory
	{
		DISALLOW_COPY(SubObjectDefinitionFactory);

	private:
		typedef ArrayMaps<IObjectDefinitionFactory*>::KeyCCString FactoryMap;
		FactoryMap _subFactories;

	public:
		SubObjectDefinitionFactory();
		~SubObjectDefinitionFactory();
		ObjectDefinition* Create(XmlNode* objNode);

		// Adds factory to available subclass factories
		// If factory with given key already exists, then old one is deleted and replaced
		void RegisterFactory(const string& typeName, IObjectDefinitionFactory* factory);
		
		// Removes factory from available subclass factories
		// If factory with given key doesn't exist - nothing happens
		void UnregisterFactory(const string& typeName);
		
		// Returns factory of given type
		// If factory with given key doesn't exist returns 0
		IObjectDefinitionFactory* GetFactoryOfType(const string& typeName);
	};
}