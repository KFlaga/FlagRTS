#pragma once

#include "GameObjectFactory.h"
#include <ArrayMap.h>
#include <Memory.h>
#include <Exception.h>

namespace FlagRTS
{
	SubClassXmlFactory::SubClassXmlFactory(const string& typeName) :
		_typeName(typeName)
	{ }

	SubClassXmlFactory::~SubClassXmlFactory()
	{
		for(auto factoryIt = _subFactories.begin(); factoryIt != _subFactories.end(); factoryIt++)
		{
			xDelete(factoryIt->Value);
		}
		_subFactories.clear();
	}

	void SubClassXmlFactory::RegisterFactory(const string& typeName, IGameObjectFactory<XmlNode*>* factory)
	{
		// Check if string have dot
		int idx = typeName.find_last_of('.');

		if(idx < string::npos)
		{
			// We have a dot
			string subType = typeName.substr(0, idx); // Get type before dot
			string factoryType = typeName.substr(idx + 1, string::npos);; // Get type after dot

			IGameObjectFactory<XmlNode*>* factory = GetFactoryOfType(subType);
			SubClassXmlFactory* subFactory = dynamic_cast<SubClassXmlFactory*>(factory);
			if(subFactory != 0)
			{
				subFactory->_subFactories.insert(factoryType.c_str(), factory);
			}
			else
			{
				CastException_d(string("SubClassXmlFactory::RegisterFactory: Error during registering factory of type: [") + 
					typeName + "]. Subtype [" + subType +"] factory is not SubClassXmlFactory");
			}
		}
		else
		{
			_subFactories.insert(typeName.c_str(), factory);
		}
	}

	void SubClassXmlFactory::UnregisterFactory(const string& typeName, IGameObjectFactory<XmlNode*>* factory)
	{
		// Check if string have dot
		int idx = typeName.find_last_of('.');

		if(idx < string::npos)
		{
			// We have a dot
			string subType = typeName.substr(0, idx); // Get type before dot
			string factoryType = typeName.substr(idx + 1, string::npos);; // Get type after dot

			IGameObjectFactory<XmlNode*>* factory = GetFactoryOfType(subType);
			SubClassXmlFactory* subFactory = dynamic_cast<SubClassXmlFactory*>(factory);
			if(subFactory != 0)
			{
				subFactory->_subFactories.erase(factoryType.c_str());
			}
			else
			{
				CastException_d(string("SubClassXmlFactory::UnregisterFactory: Error during unregistering factory of type: [") + 
					typeName + "]. Subtype [" + subType +"] factory is not SubClassXmlFactory");
			}
		}
		else
		{
			_subFactories.erase(typeName.c_str());
		}
	}

	IGameObjectFactory<XmlNode*>* SubClassXmlFactory::GetFactoryOfType(const string& typeName)
	{
		// Check if string have dot
		int idx = typeName.find_first_of('.');
		if(idx < string::npos)
		{
			// We have a dot
			string subType = typeName.substr(0, idx); // Get type before dot
			string restTypes = typeName.substr(idx + 1, string::npos); // Get types after dot

			auto subFactoryIt = _subFactories.find(subType.c_str());
			if(subFactoryIt != _subFactories.end())
			{
				SubClassXmlFactory* subFactory = dynamic_cast<SubClassXmlFactory*>(subFactoryIt->Value);
				if(subFactory != 0)
				{
					return subFactory->GetFactoryOfType(restTypes);
				}
				else
				{
					CastException_d(string("SubClassXmlFactory::GetFactoryOfType: Error during retrieving factory of type: [") + 
						typeName + "]. Subtype [" + subType +"] factory is not SubClassXmlFactory");
				}
			}
			else
			{
				CastException_d(string("SubClassXmlFactory::GetFactoryOfType: Error during retrieving factory of type: [") + 
					typeName + "]. Subtype [" + subType +"] factory is not registered");
			}
		}
		else
		{
			auto facIt = _subFactories.find(typeName.c_str());
			if(facIt != _subFactories.end())
			{
				return facIt->Value;
			}
			else
			{
				CastException(string("SubClassXmlFactory::GetFactoryOfType: Factory of type: [") + 
					typeName + "] not registered");
			}
		}
		return 0;
	}

	IGameObject* SubClassXmlFactory::Create(XmlNode* objNode)
	{
		IGameObject* object = 0;
		string name = XmlUtility::XmlGetStringIfExists(objNode, "name");

		try
		{
			string fullType = XmlUtility::XmlGetString(objNode, "type");
			IGameObjectFactory<XmlNode*>* factory = GetFactoryOfType(fullType);
			object = factory->Create(objNode);
		}
		catch(Exception* e)
		{
			throw e; // Rethrow our exception (already logged)
		}
		catch(...) // Failed to create object for some
		{
			CastException(string("SubClassXmlFactory::Create: Error during creation of") + 
				"object of type: [" + _typeName + "], name: [" + name + "].");
		}

		return object;
	}
};
