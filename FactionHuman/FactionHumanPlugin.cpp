
#include "FactionHumanPlugin.h"
#include <GameWorld.h>
#include "MetalMineClassSuppliers.h"
#include "TransporterClassSuppliers.h"
#include "PackageClassSuppliers.h"
#include "ResourceStoreSuppliers.h"
#include "PowerPlantSuppliers.h"
#include <Windows.h>
#include <UnitClassSupplier.h>
#include "EcoResources.h"
#include <Player.h>
#include "FactoryClassSuppliers.h"
#include "LogisticsCenterClassSuppliers.h"
#include <ConstructionManager.h>
#include "LogisticsCenterInRangePlanner.h"
#include <PathingSystem.h>
#include <Unit.h>
#include <MapDecorator.h>

FlagRTS::FactionHumanPlugin* ecoPlugin = 0;


namespace FlagRTS
{
	BOOL WINAPI DllEntryPoint(HINSTANCE hinstDLL, DWORD fdwReason,
		LPVOID lpReserved)
	{
		return(TRUE);
	}

	extern "C" DllExclusive DllPlugin* StartPlugin(DllPluginGlobalInstances* globals)
	{
		SetGlobalInstances(globals);
		if(ecoPlugin == 0) 
			ecoPlugin = xNew0(FactionHumanPlugin);
		return (DllPlugin*)ecoPlugin;
	}

	extern "C" DllExclusive void EndPlugin(void)
	{
		xDelete(ecoPlugin);
		ecoPlugin = 0;
		Malloc->FreeAllocator(Malloc);
	}

	struct PluginInternal
	{
		Array<IKindSpecificDataSupplier*> KindDataSups;
		Array<IObjectSpecificDataSupplier*> ObjectDataSups;
		Array<IObjectStateSupplier*> StatesSups;
		Array<IUnitCommandSupplier*> CommandsSups;
		Array<UnitClassSupplier*> ClassSups;

		EcoResources ResourcesData;

		PluginInternal() :
			ResourcesData( 8 )
		{ }

		DllExclusive void RegisterNewObject(GameWorld* gameWorld, const char* name,
			IKindSpecificDataSupplier* kds, IObjectSpecificDataSupplier* ods,
			IObjectStateSupplier* ss, IUnitCommandSupplier* cs,
			UnitClassSupplier* ucs, size_t targetType = GetTypeId<Unit>());
	};

	DllExclusive FactionHumanPlugin::FactionHumanPlugin() :
		DllGameMod(),
		_pluginInternal(xNew0(PluginInternal))
	{

	}

	DllExclusive FactionHumanPlugin::~FactionHumanPlugin()
	{
		xDelete(_pluginInternal);
	}

	DllExclusive void FactionHumanPlugin::Init()
	{
		GameWorld::GlobalWorld = _gameWorld;

		_gameWorld->SetNewResourcesData(
			_pluginInternal->ResourcesData.GetResourcesData() );

		_pluginInternal->RegisterNewObject( _gameWorld, "MetalMine",
			xNew0(MetalMineKindDataSupplier), xNew0(MetalMineObjectDataSupplier),
			xNew0(MetalMineStateSupplier), xNew0(MetalMineCommandSupplier),
			xNew0(MetalMineUnitClassSupplier));

		_pluginInternal->RegisterNewObject( _gameWorld, "Transporter",
			xNew0(TransporterKindDataSupplier), xNew0(TransporterObjectDataSupplier),
			xNew0(TransporterStateSupplier), xNew0(TransporterCommandSupplier),
			xNew0(TransporterUnitClassSupplier));

		_pluginInternal->RegisterNewObject( _gameWorld, "Package",
			xNew0(PackageKindDataSupplier), xNew0(PackageObjectDataSupplier),
			xNew0(PackageStateSupplier), 0, 0, GetTypeId<MapDecorator>());

		_pluginInternal->RegisterNewObject( _gameWorld, "ResourceStorage",
			xNew0(ResourceStoreKindDataSupplier), xNew0(ResourceStoreObjectDataSupplier),
			xNew0(ResourceStoreStateSupplier), xNew0(ResourceStoreCommandSupplier),
			xNew0(ResourceStoreUnitClassSupplier));

		_pluginInternal->RegisterNewObject( _gameWorld, "PowerPlant",
			xNew0(PowerPlantKindDataSupplier), xNew0(PowerPlantObjectDataSupplier),
			xNew0(PowerPlantStateSupplier), 0,
			xNew0(PowerPlantUnitClassSupplier));

		_pluginInternal->RegisterNewObject( _gameWorld, "Factory",
			xNew0(FactoryKindDataSupplier), xNew0(FactoryObjectDataSupplier),
			xNew0(FactoryStateSupplier),  xNew0(FactoryCommandSupplier), 0);

		_pluginInternal->RegisterNewObject( _gameWorld, "LogisticsCenter",
			xNew0(LogisticsCenterKindDataSupplier), xNew0(LogisticsCenterObjectDataSupplier),
			0, 0, 0);

		GameWorld::GlobalWorld->GetConstructionManager()->AddConstructionPlanner(
			"LCInRange", xNew2(LogisticCenterInRangePlanner,
			_gameWorld->GetOgreSceneManager(), _gameWorld->GetPathingSystem()->GetPathingMap()));
	}

	DllExclusive void FactionHumanPlugin::Deinit()
	{
		for(unsigned int i = 0; i < _pluginInternal->KindDataSups.size(); ++i)
			xDelete(_pluginInternal->KindDataSups[i]);
		for(unsigned int i = 0; i < _pluginInternal->ObjectDataSups.size(); ++i)
			xDelete(_pluginInternal->ObjectDataSups[i]);
		for(unsigned int i = 0; i < _pluginInternal->CommandsSups.size(); ++i)
			xDelete(_pluginInternal->CommandsSups[i]);
		for(unsigned int i = 0; i < _pluginInternal->StatesSups.size(); ++i)
			xDelete(_pluginInternal->StatesSups[i]);
		for(unsigned int i = 0; i < _pluginInternal->ClassSups.size(); ++i)
			xDelete(_pluginInternal->ClassSups[i]);

		_pluginInternal->KindDataSups.clear();
		_pluginInternal->ObjectDataSups.clear();
		_pluginInternal->CommandsSups.clear();
		_pluginInternal->StatesSups.clear();
		_pluginInternal->ClassSups.clear();
	}

	DllExclusive void PluginInternal::RegisterNewObject(GameWorld* gameWorld, const char* name,
		IKindSpecificDataSupplier* kds, IObjectSpecificDataSupplier* ods,
		IObjectStateSupplier* ss, IUnitCommandSupplier* cs,
		UnitClassSupplier* ucs, size_t targetType)
	{
		if( kds !=0 && ods != 0 )
		{
			gameWorld->RegisterDataSupplier(name, kds, ods);
			KindDataSups.push_back(kds);
			ObjectDataSups.push_back(ods);
		}
		if( ss != 0)
		{
			gameWorld->RegisterObjectStatesSupplier(name, targetType, ss);
			StatesSups.push_back(ss);
		}
		if( cs != 0 )
		{
			gameWorld->RegisterUnitCommandsSupplier(name, cs);
			CommandsSups.push_back(cs);
		}
		if( ucs != 0 )
		{
			gameWorld->RegisterUnitClassSupplier(name, ucs);
			ClassSups.push_back(ucs);
		}
	}
}
