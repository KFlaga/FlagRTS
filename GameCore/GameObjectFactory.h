#pragma once

#include "IGameObject.h"
#include <Xml.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	/// Interface for generic IGameObject factory
	/**
		Creates IGameObject via call to 'Create()'
		Template parameter specfies class that is used to create final object
		If more than one parameter is required, use dedicated data struct
	 */
	template<typename CreationParameter>
	class IGameObjectFactory
	{
	public:
		IGameObjectFactory() { }
		virtual ~IGameObjectFactory() { }

		virtual IGameObject* Create(CreationParameter parameter) = 0;
	};
}

namespace FlagRTS
{
	/// Factory base for non-final types created from Xml definitions
	/**	
		Base class for factories creating IGameObject of non-final types from Xml definitions.
		Creation of final object is delegated to sub types factories
	 
		Assumes that 
		It assumes that every base type node have attribute 'type' with name of next subclass type
		If this will change only place where changes are needed in code is corresponding cpp file
	*/
	class SubClassXmlFactory : public IGameObjectFactory<XmlNode*>
	{
		DISALLOW_COPY(SubClassXmlFactory);

	private:
		typedef ArrayMaps<IGameObjectFactory<XmlNode*>*>::KeyCCString FactoryMap;
		FactoryMap _subFactories;
		string _typeName;

	public:
		SubClassXmlFactory(const string& typeName);
		~SubClassXmlFactory();
		IGameObject* Create(XmlNode* objNode);

		// Adds factory to available subclass factories 
		// If 'typeName' contains dots then request is passed to subfactory with type before first dot
		// ( like base_class.sub_class.subsub_class )
		// If factory with given key already exists, then old one is deleted and replaced
		void RegisterFactory(const string& typeName, IGameObjectFactory<XmlNode*>* factory);
		
		// Removes factory from available subclass factories
		// If 'typeName' contains dots then request is passed to subfactory with type before first dot
		// ( like base_class.sub_class.subsub_class )
		// If factory with given key doesn't exist - nothing happens
		void UnregisterFactory(const string& typeName, IGameObjectFactory<XmlNode*>* factory);
		
		// Returns factory of given type
		// If 'typeName' contains dots then request is passed to subfactory with type before first dot
		// ( like base_class.sub_class.subsub_class )
		// If factory with given key doesn't exist returns 0
		IGameObjectFactory<XmlNode*>* GetFactoryOfType(const string& typeName);
	};
	
	// Factory base for final types created from Xml definitions
	// Needs not to be inhertied, but rather added to SubClass factories
	template<typename ObjType>
	class FinalClassXmlFactory : public IGameObjectFactory<XmlNode*>
	{
		IGameObject* Create(XmlNode* objNode) 
		{
			return xNew1(ObjType, objNode); 
		}
	};
}