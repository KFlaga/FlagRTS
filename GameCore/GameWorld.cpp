#include "GameWorld.h"
#include <ErrorCodes.h>
#include <LogManager.h>
#include <OgreRenderWindow.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include "Map.h"
#include "SceneObjectPools.h"
#include "SpawnInfoFactory.h"
#include "PathingSystem.h"
#include <PathFindingUniformGridPathingMap.h>
#include "IKindSpecificDataSupplier.h"
#include "Player.h"
#include "Resources.h"
#include <ArrayMap.h>
#include "ConstructionManager.h"
#include "UnitClass.h"
#include "PlayerStatsManager.h"
#include "FactionsManager.h"
#include "TechnologyManager.h"
#include "NoticeMessages.h"
#include <StringUtils.h>
#include "CollisionGroup.h"
#include "Minimap.h"
#include "InGameSettings.h"

#include "IObjectDefinitionManager.h"
#include "IGameObjectPoolManager.h"
#include "IGameObjectPool.h"
#include "ISceneObjectSpawner.h"

#include "ObjectDataStorage.h"

namespace FlagRTS
{
	GameWorld* GameWorld::GlobalWorld = 0;

	struct SuppliersData
	{
		Array<IKindSpecificDataSupplier*> KindDataSups;
		Array<IObjectSpecificDataSupplier*> ObjectDataSups;
		Array<const char*> DataSupsNames;
		Array<IObjectStateSupplier*> StatesSups;
		Array<std::pair<const char*, size_t>> StatesSupsTargets;
		Array<IUnitCommandSupplier*> CommandsSups;
		Array<const char*> CommandsSupsNames;
		Array<UnitClassSupplier*> ClassSups;
		Array<const char*> ClassSupsNames;
	};

	GameWorld::GameWorld() :
		_ogreSceneMgr(0),
		_objectDefinitionManager(0),
		_objectPoolManager(0),
		_mainObjectPool(0),
		_sceneObjectSpawner(0),
		_noticeSender(0),
		_statsManager(0),
		_map(0),
		_pathing(0),
		_techsManager(0),
		_factionsManager(0),
		_players(0),
		_resources(0),
		_minimap(0)
	{

	}

	GameWorld::GameWorld(Ogre::SceneManager* ogreMgr,
			IObjectDefinitionManager* objectDefinitionManager,
			IGameObjectPoolManager* objectPoolManager,
			IGameObjectPool* mainObjectPool,
			ISceneObjectSpawner* sceneObjSpawner,
			INoticeMessageSender* noticeSender,
			GlobalStatisticsManager* statsMgr ) :
		_ogreSceneMgr(ogreMgr),
		_objectDefinitionManager(objectDefinitionManager),
		_objectPoolManager(objectPoolManager),
		_mainObjectPool(mainObjectPool),
		_sceneObjectSpawner(sceneObjSpawner),
		_noticeSender(noticeSender),
		_statsManager(statsMgr),
		_map(xNew1(Map, ogreMgr)),
		_pathing(0),
		_techsManager(0),
		_factionsManager(0),
		_players(xNew0(PlayersInfo)),
		_resources(xNew0(Resources)),
		_minimap(0)
	{ 
		//TODO: create default implementation of interfaces if any is 0

		//_objectDataStorage = xNew0(ObjectDataStorage);
	}

	GameWorld::~GameWorld()
	{
		// Delete spawn infos
		for(auto objIt = _mapInitObjects.begin(); objIt != _mapInitObjects.end(); ++objIt)
		{
			xDelete(objIt->second);
		}
		/*xDeleteSafe(_noticeSender);
		xDeleteSafe(_sceneObjectSpawner);
		xDeleteSafe(_objectDefinitionManager);
		xDeleteSafe(_mainObjectPool);
		xDeleteSafe(_objectPoolManager);*/
		
		xDeleteSafe(_registeredSups);
		xDeleteSafe(_factionsManager);
		xDeleteSafe(_techsManager);
		xDeleteSafe(_minimap);
		xDeleteSafe(_constructionMgr);
		xDeleteSafe(_pathing);
		xDeleteSafe(_map);
		xDeleteSafe(_players);
		xDeleteSafe(_resources);
	//	xDeleteSafe(_objectDataStorage);
	}

	void GameWorld::Update(float ms)
	{
		_minimap->Update(ms);
	}

	void GameWorld::SetGameSettings(InGameSettings* settings)
	{
		_gameSettings = settings;
		_players->SetPlayersSetting(_gameSettings);
	}

	void GameWorld::LoadSpawnInfos()
	{
		XmlNode* mapMainNode = _gameSettings->MapFile->first_node("Map",3);
		XmlNode* spawnNode = mapMainNode->first_node("SceneObjects");

		SpawnInfoFactory spFactory;
		XmlNode* objNode = spawnNode->first_node("SceneObject");
		// For each SceneObject to be spawned create spawn info
		while(objNode != 0)
		{
			SpawnInfo* spawnInfo = (SpawnInfo*)spFactory.Create(objNode);
			XmlNode* ownerNode = objNode->first_node("Owner");
			int owner = XmlUtility::XmlGetInt(ownerNode, "value");
			spawnInfo->SetOwner(owner);
			if( _players->GetPlayer(owner)->IsUsed() )
			{
				string defName = objNode->first_attribute("name",4,false)->value();
				string defType = objNode->first_attribute("type",4,false)->value();
				// Pair spawn info with SODefinition of object to be spawned
				_mapInitObjects.push_back(std::make_pair(
					spawnInfo,
					static_cast<SceneObjectDefinition*>(
					_objectDefinitionManager->GetObjectDefinitionByName(defName,defType))));
			}
			// Get next SceneObject
			objNode = objNode->next_sibling("SceneObject");
		}
	}

	void GameWorld::LoadTerrain()
	{
		XmlNode* mapNode = _gameSettings->MapFile->first_node("Map");
		_map->InitTerrain(mapNode);
	}

	void GameWorld::CreateGameManagers()
	{
		CreatePathingSystem();

		_constructionMgr = xNew2(ConstructionManager,
			_ogreSceneMgr, _pathing->GetPathingMap());

		_factionsManager = xNew0(FactionsManager);

		_techsManager = xNew1(GlobalTechnologyManager, _players->GetPlayersCount());
	}

	void GameWorld::CreateMinimap()
	{
		XmlNode* mapNode = _gameSettings->MapFile->first_node("Map",3);

		string mapPreiviewImageName = XmlUtility::XmlGetString(
			mapNode->first_node("MinimapImage"), "name");

		_minimap = xNew4(Minimap, _map->GetWorldSizeOfTerrain(), 
			IntVector2(512, (int)(512.f * (_map->GetWorldSizeOfTerrain().y / _map->GetWorldSizeOfTerrain().x))),
			mapPreiviewImageName, "Maps");

		_minimap->SetUseTerrainTexture();
	}

	void GameWorld::CreatePathingSystem()
	{
		auto& terrInfo = _map->GetTerrain()->GetTerrainInfo();
		_pathing = xNew4(PathingSystem,
			_ogreSceneMgr,
			terrInfo.GetTilesX()*terrInfo.GetCellCountInTile(), 
			terrInfo.GetTilesZ()*terrInfo.GetCellCountInTile(),
			_map->GetWorldSizeOfCell());
		_pathing->SetRootDebugNode(_ogreSceneMgr->getRootSceneNode());
	}

	void GameWorld::CreateWorld()
	{
		_map->LoadAndSpawnTerrain();

		CreateMinimap();

		// As terrains are loaded, may create terrain obstacles
		_pathing->CreateSlopeObstacles(_map->GetTerrain());

		// Set players technology
		for(int i = 0; i < _players->GetPlayersCount(); ++i)
		{
			auto playerTechsMgr = _techsManager->GetPlayerTechs((uint8)i);
			playerTechsMgr->SetFactionTechnology( 
				_factionsManager->GetFaction(_players->GetPlayer(i)->GetFaction()));
		}

		// Set resources info
		_resources->SetResourcesSettings(_gameSettings);

		// Set fog of war etc.

		// Create and spawn all objects
		for(auto objIt = _mapInitObjects.begin(); objIt != _mapInitObjects.end(); objIt++)
		{
			SceneObject* obj = static_cast<SceneObject*>(GetGameObjectPool()->Create(
				objIt->second, objIt->first->GetOwner()));
			GetSceneObjectSpawner()->SpawnSceneObject(obj, *objIt->first);
		}
	}

	void GameWorld::ShowWorld()
	{
		_minimap->BeginRender();
	}

	void GameWorld::HideWorld()
	{
		_minimap->EndRender();
	}

	void GameWorld::ResetWorld()
	{

	}

	void GameWorld::UnloadMap()
	{
		_map->DestroyMap();
	}

	//void GameWorld::SpawnSceneObject(SceneObject* object, 
	//	const SpawnInfo& spawnInfo)
	//{
	//	_map->SpawnObject(object, spawnInfo);
	//	if(object->HavePhysics())
	//	{
	//		if(object->HaveFootprint())
	//		{
	//			_pathing->AddObstacle(static_cast<PhysicalObject*>(object));
	//		}
	//		_pathing->AddPathingObject(static_cast<PhysicalObject*>(object));
	//	}

	//	int mmapFlags = object->GetMinimapFlags();
	//	if( mmapFlags != MinimapFlags::None &&
	//		(mmapFlags & MinimapFlags::NeverVisible) == 0 )
	//	{
	//		// Object is to be shown on minimap, so add it to minimap
	//		// But first set it owner flag
	//		if( object->GetOwner() == NEUTRAL_PLAYERNUM )
	//			mmapFlags |= MinimapFlags::EnvironmentUnit;
	//		else if( _players->IsOwned(object->GetOwner()) )
	//			mmapFlags |= MinimapFlags::OwnedUnit;
	//		else if( _players->IsAlly(_players->GetClientPlayer(), object->GetOwner()) )
	//			mmapFlags |= MinimapFlags::AllyUnit;
	//		else if( _players->IsNeutral(_players->GetClientPlayer(), object->GetOwner()) )
	//			mmapFlags |= MinimapFlags::NeutralUnit;
	//		else if( _players->IsEnemy(_players->GetClientPlayer(), object->GetOwner()) )
	//			mmapFlags |= MinimapFlags::EnemyUnit;

	//		object->SetMinimapFlags(mmapFlags);

	//		_minimap->AddObject(object);
	//	}

	//	if(object->GetFinalType() == GetTypeId<Unit>() &&
	//		static_cast<Unit*>(object)->CountsAsPlayerUnit() &&
	//		static_cast<Unit*>(object)->GetOwner() < 8u) // If owner >= 8 it belongs to neutral player ( environment ), so don't add it to stats
	//	{
	//		_statsManager->UnitSpawned(  static_cast<Unit*>(object) );
	//	}
	//}

	/*void GameWorld::DespawnSceneObject(SceneObject* object)
	{
		if(object->HavePhysics())
		{
			if(object->HaveFootprint())
			{
				_pathing->RemoveObstacle(static_cast<PhysicalObject*>(object));
			}
			_pathing->RemovePathingObject(static_cast<PhysicalObject*>(object));
		}
		_map->DespawnObject(object);

		if(object->GetFinalType() == GetTypeId<Unit>())
		{
			Unit* unit = static_cast<Unit*>(object);
			if( unit->CountsAsPlayerUnit() && unit->GetOwner() <= 8 && !unit->IsDead() )
				_statsManager->UnitVanished( unit );
		}
	}*/

	void GameWorld::SetNewResourcesData(Resources* resources)
	{
		if(_resources != 0)
		{
			xDelete(_resources);
			_resources = resources;
		}
	}

	void GameWorld::ActivateAllRegisteredSuppliers()
	{
		//for(unsigned int dataSup = 0; dataSup < _registeredSups->KindDataSups.size(); ++dataSup)
		//{
		//	IKindSpecificDataSupplier* kdsup =	_registeredSups->KindDataSups[dataSup];
		//	IObjectSpecificDataSupplier* odsup = _registeredSups->ObjectDataSups[dataSup];
		//	string name = _registeredSups->DataSupsNames[dataSup];
		//	auto setKindData = [ this, name, kdsup, odsup ](IObjectDefinitionManager* mgr) 
		//	{
		//		// For each definition check if it have given ObjectData name
		//		auto allDefs = mgr->GetAllDefinitions();
		//		for(unsigned int i = 0; i < allDefs->size(); ++i)
		//		{
		//			auto& def = (*allDefs)[i];
		//			if( def->GetObjectDataName().compare(name) == 0 )
		//			{
		//				def->SetKindDataHandleSupplier(kdsup);
		//				def->SetObjectDataHandleSupplier(odsup);
		//				def->SetKindSpecificDataHandle(kdsup->GetKindSpecificDataHandle());
		//			}
		//		}
		//	};

		//	typedef DelegateEventHandler<decltype(setKindData), IObjectDefinitionManager*> DefinitionsLoadedHandler;
		//	_objectPool->OnAllDefinitionsLoaded() +=
		//		xNew1(DefinitionsLoadedHandler, setKindData);
		//}

		/*UnitPool* unitPool = (UnitPool*)_objectPool->GetObjectPool(GetTypeId<Unit>());

		for(unsigned int sup = 0; sup < _registeredSups->StatesSups.size(); ++sup)
		{
			IObjectStateSupplier* supplier = _registeredSups->StatesSups[sup];
			const char* name = _registeredSups->StatesSupsTargets[sup].first;

			_objectPool->GetObjectPool(_registeredSups->StatesSupsTargets[sup].second)->
				RegisterObjectStateSupplier(name, supplier);
		}

		for(unsigned int sup = 0; sup < _registeredSups->CommandsSups.size(); ++sup)
		{
			IUnitCommandSupplier* supplier = _registeredSups->CommandsSups[sup];
			const char* name = _registeredSups->CommandsSupsNames[sup];

			unitPool->RegisterCommandSupplier(name, supplier);
		}

		for(unsigned int sup = 0; sup < _registeredSups->ClassSups.size(); ++sup)
		{
			UnitClassSupplier* supplier = _registeredSups->ClassSups[sup];
			const char* name = _registeredSups->ClassSupsNames[sup];

			unitPool->RegisterUnitClassSupplier(name, supplier);
		}*/
	}

	void GameWorld::DeactivateAllRegisteredSuppliers()
	{
		/*UnitPool* unitPool = (UnitPool*)_objectPool->GetObjectPool(GetTypeId<Unit>());

		for(unsigned int sup = 0; sup < _registeredSups->DataSupsNames.size(); ++sup)
		{
			sDeleteArray(_registeredSups->DataSupsNames[sup]);
		}

		for(unsigned int sup = 0; sup < _registeredSups->StatesSups.size(); ++sup)
		{
			const char* name = _registeredSups->StatesSupsTargets[sup].first;

			_objectPool->GetObjectPool(_registeredSups->StatesSupsTargets[sup].second)->
				UnregisterObjectStateSupplier(name);

			sDeleteArray(_registeredSups->StatesSupsTargets[sup].first);
		}

		for(unsigned int sup = 0; sup < _registeredSups->CommandsSups.size(); ++sup)
		{
			const char* name = _registeredSups->CommandsSupsNames[sup];
			unitPool->UnregisterCommandSupplier(name);
			sDeleteArray(_registeredSups->CommandsSupsNames[sup]);
		}

		for(unsigned int sup = 0; sup < _registeredSups->CommandsSups.size(); ++sup)
		{
			const char* name = _registeredSups->ClassSupsNames[sup];
			unitPool->UnregisterCommandSupplier(name);
			sDeleteArray(_registeredSups->ClassSupsNames[sup]);
		}

		_registeredSups->CommandsSups.clear();
		_registeredSups->CommandsSupsNames.clear();
		_registeredSups->DataSupsNames.clear();
		_registeredSups->KindDataSups.clear();
		_registeredSups->ObjectDataSups.clear();
		_registeredSups->StatesSups.clear();
		_registeredSups->StatesSupsTargets.clear();
		_registeredSups->ClassSups.clear();
		_registeredSups->ClassSupsNames.clear();*/
	}

	void GameWorld::RegisterDataSupplier(const string& supname,
		IKindSpecificDataSupplier* kdsup,
		IObjectSpecificDataSupplier* odsup)
	{
		_registeredSups->KindDataSups.push_back(kdsup);
		_registeredSups->ObjectDataSups.push_back(odsup);

		_registeredSups->DataSupsNames.push_back(CopyString(supname));
	}

	void GameWorld::RegisterObjectStatesSupplier(const string& name, 
		size_t targetObjectsTypeId, IObjectStateSupplier* supplier)
	{
		_registeredSups->StatesSups.push_back(supplier);

		_registeredSups->StatesSupsTargets.push_back(std::make_pair(
			CopyString(name), targetObjectsTypeId));
	}

	void GameWorld::RegisterUnitCommandsSupplier(const string& name, IUnitCommandSupplier* supplier)
	{
		_registeredSups->CommandsSups.push_back(supplier);

		_registeredSups->CommandsSupsNames.push_back(CopyString(name));
	}

	void GameWorld::RegisterUnitClassSupplier(const string& name, UnitClassSupplier* supplier)
	{
		_registeredSups->ClassSups.push_back(supplier);

		_registeredSups->ClassSupsNames.push_back(CopyString(name));
	}
}