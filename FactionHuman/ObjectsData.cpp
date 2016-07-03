#include "MetalMineData.h"
#include "TransporterData.h"
#include "ResourceStoreData.h"
#include "PackageData.h"
#include <GameWorld.h>
#include <MainGameObjectPool.h>
#include "PowerPlantData.h"
#include "FactoryData.h"
#include "LogisticsCenterData.h"
#include <SceneObject.h>


namespace FlagRTS
{
	DllExclusive MetalMineTypeData::MetalMineTypeData() :
		PackageWaitTime(5000.f),
		PackageDefinitionHandle(),
		PackageMetalAmount(500),
		TransporterDriveInSpot(Vector3(-10.f, 0.f, 120.f)),
		PackageSpawnSpot(Vector3(25.f,2.5f,-20.f)),
		PackageFinalSpot(Vector3(25.f,2.5f,20.f))
	{
		// Load above values from mod config file if needed
		PackageDefinitionHandle = GameWorld::GlobalWorld->GetSceneObjectDefinition(
			"MapDecorator", "MinePackage")->GetHandle();
	}

	DllExclusive MetalMineObjectData::MetalMineObjectData() :
		PackageTimer(0.0f),
		AvailableMetal(10000),
		Transporter(0),
		Package(0),
		PackageReady(false),
		OnTransporterDied(this)
	{ 
		// Load above values from mod config file if needed

	}

	DllExclusive TransporterTypeData::TransporterTypeData() :
		TransporterPackageSpot(Vector3(0.0f, 13.1f, -1.0f)),
		AutoLookForMine(true),
		AutoMineTime(3000.f)
	{ }

	DllExclusive TransporterObjectData::TransporterObjectData() :
		TargetMine(0),
		TargetStore(0),
		Package(0),
		OnMineDied(this),
		OnStoreDied(this),
		IsStorePlayerChoosen(false)
	{ }

	DllExclusive ResourceStoreTypeData::ResourceStoreTypeData() :
		DoorSpot(Vector3(12.0f, 0.f, 85.f)),
		InsideSpot(Vector3(12.0f, 0.f, 20.f))
	{

	}

	DllExclusive ResourceStoreObjectData::ResourceStoreObjectData() :
		Transporter(0),
		DoorsOpened(false),
		DoorsClosed(true),
		OnTransporterDied(this)
	{ 
		
	}

	DllExclusive PackageTypeData::PackageTypeData()
	{
		
	}

	DllExclusive PackageObjectData::PackageObjectData() :
		StoredMetal(0)
	{}
	
	DllExclusive PowerPlantTypeData::PowerPlantTypeData() :
		TickLength(1000.f)
	{
		
	}

	DllExclusive PowerPlantObjectData::PowerPlantObjectData() :
		EnergyPerTick(100)
	{}

	DllExclusive FactoryTypeData::FactoryTypeData() :
		DoorCenter(20.f,0.f,35.f),
		DoorsHalfLength(28.f),
		InsideSpot(20.f,0.f,0.f)
	{
		
	}

	DllExclusive FactoryObjectData::FactoryObjectData() :
		UnitInside(0),
		DoorsOpened(false),
		DoorsClosed(true)
	{}

	DllExclusive LogisticsCenterTypeData::LogisticsCenterTypeData() :
		BaseInfluenceRadius(600.f)
	{
		
	}

	DllExclusive LogisticsCenterObjectData::LogisticsCenterObjectData() :
		CurrentInfluenceRadius(600.f),
		CurrentSquaredInfluenceRadius(360000.f)
	{}
}
