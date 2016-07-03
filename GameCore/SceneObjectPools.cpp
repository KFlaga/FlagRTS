#include "SceneObjectPools.h"
#include "DefaultStateSuppliers.h"
#include "DefaultCommandsSuppliers.h"
#include <Exception.h>
#include "UnitClass.h"
#include <Profiler.h>

namespace FlagRTS
{
	using namespace UnitClasses;

	MapDecoratorPool::MapDecoratorPool()
	{
		// Register default map decorator state suppliers
		RegisterObjectStateSupplier("NoPrespawn", xNew0(MapDecoratorNoPrespawnStateSupplier));
		RegisterObjectStateSupplier("Classless", xNew0(MapDecoratorNoPrespawnStateSupplier));
	}

	MapDecoratorPool::~MapDecoratorPool()
	{

	}

	MapDecorator* MapDecoratorPool::CreateCast(MapDecoratorDefinition* objDef, int owner)
	{
		MapDecorator* newObj(xNew1(MapDecorator, objDef));
		_objects.insert(newObj->GetHandle().Object, newObj);
		newObj->SetOwner(owner);
		AddStatesToObject(newObj, objDef);

		return newObj;
	}


	void MapDecoratorPool::DestroyCast(MapDecorator* object)
	{
		_objects.erase(object->GetHandle().Object);
		xDelete(object);
	}

	GameCameraPool::GameCameraPool()
	{
		RegisterObjectStateSupplier("Classless", xNew0(GameCameraDefaultStateSupplier));
	}

	GameCameraPool::~GameCameraPool()
	{

	}

	GameCamera* GameCameraPool::CreateCast(GameCameraDefinition* objDef, int owner)
	{
		GameCamera* newObj(xNew1(GameCamera, objDef));
		_objects.insert(newObj->GetHandle().Object, newObj);
		newObj->SetOwner(owner);
		AddStatesToObject(newObj, objDef);

		return newObj;
	}

	void GameCameraPool::DestroyCast(GameCamera* object)
	{
		_objects.erase(object->GetHandle().Object);
		xDelete(object);
	}

	UnitPool::UnitPool()
	{
		RegisterObjectStateSupplier("Base", xNew0(UnitBaseStateSupplier));
		RegisterObjectStateSupplier("MoveGround", xNew0(UnitMoveGroundStateSupplier));
		RegisterObjectStateSupplier("Rotateable", xNew0(UnitMoveGroundStateSupplier));
		RegisterObjectStateSupplier("AttackGround", xNew0(UnitAttackGroundStateSupplier));
		RegisterObjectStateSupplier("Aggressive", xNew0(UnitAggresiveStateSupplier));
		RegisterObjectStateSupplier("Passive", xNew0(UnitPassiveStateSupplier));
		RegisterObjectStateSupplier("Building", xNew0(BuildingStateSupplier));
		RegisterObjectStateSupplier("ConstructionSite", xNew0(ConstructionSiteStateSupplier));
		
		RegisterCommandSupplier("Base", xNew0(BaseCommandSupplier));
		RegisterCommandSupplier("AttackGround", xNew0(AttackGroundCommandSupplier));
		RegisterCommandSupplier("MoveGround", xNew0(MoveGroundCommandSupplier));
		RegisterCommandSupplier("TrainUnits", xNew0(TrainUnitCommandSupplier));
		RegisterCommandSupplier("Rotateable", xNew0(RotateCommandSupplier));

		RegisterUnitClassSupplier("Aggressive",	new UnitClassSupplier( 
			CreateClass(SubClasses::Aggresive),
			CreateClass(SubClasses::Retreat, SubClasses::Passive)));
		RegisterUnitClassSupplier("MoveGround", new UnitClassSupplier( 
			CreateClass(SubClasses::Ground, SubClasses::Mobile),
			CreateClass(SubClasses::Immobile)));
		RegisterUnitClassSupplier("Passive", new UnitClassSupplier( 
			CreateClass(SubClasses::Passive),
			CreateClass(SubClasses::Retreat, SubClasses::Aggresive)));
		RegisterUnitClassSupplier("AttackGround", new UnitClassSupplier( 
			CreateClass(SubClasses::AttackGround, SubClasses::AttackBuilding),
			ComplexClasses::None));
		RegisterUnitClassSupplier("Building", new UnitClassSupplier( 
			ComplexClasses::Building,
			ComplexClasses::None));
		RegisterUnitClassSupplier("ConstructionSite", new UnitClassSupplier( 
			CreateClass(SubClasses::ConstructionSite),
			ComplexClasses::None));
		RegisterUnitClassSupplier("TrainUnits", new UnitClassSupplier( 
			CreateClass(SubClasses::TrainUnits),
			ComplexClasses::None));
	}

	UnitPool::~UnitPool()
	{
		for(auto it = _commandSuppliers.begin(); it != _commandSuppliers.end(); ++it)
		{
			xDelete(it->Value);
		}
		for(auto it = _classSuppliers.begin(); it != _classSuppliers.end(); ++it)
		{
			xDelete(it->Value);
		}
	}

	Unit* UnitPool::CreateCast(UnitDefinition* objDef, int owner)
	{
		Unit* newObj(xNew1(Unit, objDef));
		_objects.insert(newObj->GetHandle().Object, newObj);
		newObj->SetOwner(owner);

		_stateSuppliers["Base"]->AddStatesToObject(newObj);
		_commandSuppliers["Base"]->AddCommandsToUnit(newObj);

		// Add states and commands to object
		for(auto classNameIt = objDef->GetObjectClasses().begin();
			classNameIt != objDef->GetObjectClasses().end(); ++classNameIt)
		{
			auto suppIt = _stateSuppliers.find(*classNameIt);
			if(suppIt != _stateSuppliers.end())
				suppIt->Value->AddStatesToObject(newObj);

			auto commIt = _commandSuppliers.find(*classNameIt);
			if(commIt != _commandSuppliers.end())
				commIt->Value->AddCommandsToUnit(newObj);

			auto classIt = _classSuppliers.find(*classNameIt);
			if(classIt != _classSuppliers.end())
			{
				newObj->SetUnitClass(classIt->Value->
					ChangeUnitClass(newObj->GetUnitClass()));
			}
		}

		// Add command buttons from definiton
		for(auto cbutIt = objDef->GetDefaultCommandButtons().begin(); 
			cbutIt != objDef->GetDefaultCommandButtons().end(); ++cbutIt)
		{
			newObj->GetCommands()->SetCommandButton(cbutIt->first, cbutIt->second);
		}

		return newObj;
	}

	//Unit* UnitPool::CreateUnit(UnitDefinition* objDef, uint8 owner)
	//{
	//	PROFILE_REGISTER_CLOCK(createClock, "UnitPool::CreateUnit");
	//	PROFILE_START(createClock);

	//	Unit* newObj(xNew1(Unit, objDef));
	//	newObj->SetOwner(owner);
	//	_objects.insert(newObj->GetHandle().Object, newObj);
	//	
	//	_stateSuppliers["Base"]->AddStatesToObject(newObj);
	//	_commandSuppliers["Base"]->AddCommandsToUnit(newObj);

	//	// Add states and commands to object
	//	for(auto classNameIt = objDef->GetObjectClasses().begin();
	//		classNameIt != objDef->GetObjectClasses().end(); ++classNameIt)
	//	{
	//		auto suppIt = _stateSuppliers.find(*classNameIt);
	//		if(suppIt != _stateSuppliers.end())
	//			suppIt->Value->AddStatesToObject(newObj);

	//		auto commIt = _commandSuppliers.find(*classNameIt);
	//		if(commIt != _commandSuppliers.end())
	//			commIt->Value->AddCommandsToUnit(newObj);

	//		auto classIt = _classSuppliers.find(*classNameIt);
	//		if(classIt != _classSuppliers.end())
	//		{
	//			newObj->SetUnitClass(classIt->Value->
	//				ChangeUnitClass(newObj->GetUnitClass()));
	//		}
	//	}

	//	// Add command buttons from definiton
	//	for(auto cbutIt = objDef->GetDefaultCommandButtons().begin(); 
	//		cbutIt != objDef->GetDefaultCommandButtons().end(); ++cbutIt)
	//	{
	//		newObj->GetCommands()->SetCommandButton(cbutIt->first, cbutIt->second);
	//	}

	//	PROFILE_END(createClock);

	//	return newObj;
	//}

	void UnitPool::DestroyCast(Unit* object)
	{
		_objects.erase(object->GetHandle().Object);
		xDelete(object);
	}

	MisslePool::MisslePool()
	{
		RegisterObjectStateSupplier("Classless", xNew0(MissleDefaultStateSupplier));
	}

	MisslePool::~MisslePool()
	{

	}

	Missle* MisslePool::CreateCast(MissleDefinition* objDef, int owner)
	{
		Missle* newObj(xNew1(Missle, objDef));
		_objects.insert(newObj->GetHandle().Object, newObj);
		newObj->SetOwner(owner);
		AddStatesToObject(newObj, objDef);

		return newObj;
	}


	void MisslePool::DestroyCast(Missle* object)
	{
		_objects.erase(object->GetHandle().Object);
		xDelete(object);
	}
}