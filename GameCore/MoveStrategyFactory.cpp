#include "MoveStrategyFactory.h"
#include "DefaultMoveStrategies.h"
#include <LogManager.h>
#include <Exception.h>

namespace FlagRTS
{
	DEFINE_FINAL_FACTORY(NoMoveMoveStrategyFactory, 
		NoMoveMoveStrategy, 
		"NoMoveMoveStrategy", 
		XmlNode*);
	
	DEFINE_FINAL_FACTORY(FaceAndGoMoveStrategyFactory, 
		FaceAndGoMoveStrategy, 
		"FaceAndGoMoveStrategy", 
		XmlNode*);
	
	DEFINE_FINAL_FACTORY(RotateOnlyStrategyFactory, 
		RotateOnlyStrategy, 
		"RotateOnlyStrategy", 
		XmlNode*);
	

	MoveStrategyFactory::MoveStrategyFactory() :
		IFactoryCast("IMover")
	{
		_subtypeFactories.insert(std::make_pair("NoMove",
			xNew0(NoMoveMoveStrategyFactory)));

		_subtypeFactories.insert(std::make_pair("FaceAndGo",
			xNew0(FaceAndGoMoveStrategyFactory)));
		
		_subtypeFactories.insert(std::make_pair("RotateOnly",
			xNew0(RotateOnlyStrategyFactory)));
	}

	MoveStrategyFactory::~MoveStrategyFactory()
	{
		for(auto factoryIt = _subtypeFactories.begin(); factoryIt != _subtypeFactories.end(); factoryIt++)
		{
			xDelete(factoryIt->Value);
		}
		_subtypeFactories.clear();
	}

	IGameObject* MoveStrategyFactory::Create(XmlNode* objNode)
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
			CastException((string("Bad move strategy type : ") + typeName).c_str());
		}

		return obj;
	}
}
