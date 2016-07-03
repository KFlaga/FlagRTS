#include "MoverFactory.h"
#include "Movers.h"
#include <LogManager.h>
#include <Exception.h>

namespace FlagRTS
{
	DEFINE_FINAL_FACTORY(LineMoverFactory, 
		LineMover, 
		"LineMover", 
		XmlNode*);
	
	DEFINE_FINAL_FACTORY(AcceleratedLineMoverFactory, 
		AcceleratedLineMover, 
		"AcceleratedLineMover", 
		XmlNode*);
	

	MoverFactory::MoverFactory() :
		IFactoryCast("IMover")
	{
		_subtypeFactories.insert(std::make_pair("Line",
			xNew0(LineMoverFactory)));

		_subtypeFactories.insert(std::make_pair("AcceleratedLine",
			xNew0(AcceleratedLineMoverFactory)));
	}

	MoverFactory::~MoverFactory()
	{
		for(auto factoryIt = _subtypeFactories.begin(); factoryIt != _subtypeFactories.end(); factoryIt++)
		{
			xDelete(factoryIt->Value);
		}
		_subtypeFactories.clear();
	}

	IGameObject* MoverFactory::Create(XmlNode* objNode)
	{
		IGameObject* obj = 0;

		// First find higher level type of object and corretponding factory
		char* typeName = objNode->first_attribute("type", 4, false)->value();
		auto subFactory = _subtypeFactories.find(typeName);
		if(subFactory != _subtypeFactories.end()) // If subtype factory found
		{
			// Delegate creation of final object to subtype factory
			obj = subFactory->Value->Create(objNode);
		}
		else
		{
			CastException((string("Bad mover type : ") + typeName).c_str());
		}

		return obj;
	}
}
