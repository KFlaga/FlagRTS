#include "GameWorld.h"
#include <ErrorCodes.h>
#include <LogManager.h>
#include <OgreRenderWindow.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include "Map.h"
#include "MainGameObjectPool.h"
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

	GameWorld::GameWorld(Ogre::SceneManager* ogreMgr, 
		Ogre::RenderWindow* gameWindow, 
		MainGameObjectPool* objectPool,
		GlobalStatisticsManager* statsMgr
		) :
	_ogreSceneMgr(ogreMgr),
		_gameWindow(gameWindow),
		_objectPool(objectPool),
		_statsManager(statsMgr),
		_map(xNew1(Map, ogreMgr)),
		_pathing(0),
		_registeredSups(xNew0(SuppliersData)),
		_techsManager(0),
		_factionsManager(0),
		_players(xNew0(PlayersInfo)),
		_resources(xNew0(Resources)),
		_minimap(0)
	{ }

	GameWorld::~GameWorld()
	{
		// Delete spawn infos
		for(auto objIt = _mapInitObjects.begin(); objIt != _mapInitObjects.end(); ++objIt)
		{
			xDelete(objIt->first.first);
		}
		xDeleteSafe(_registeredSups);
		xDeleteSafe(_factionsManager);
		xDeleteSafe(_techsManager);
		xDeleteSafe(_minimap);
		xDeleteSafe(_constructionMgr);
		xDeleteSafe(_pathing);
		xDeleteSafe(_map);
		xDeleteSafe(_players);
		xDeleteSafe(_resources);
	}

	void GameWorld::Update(float ms)
	{
		// Execute all pending requests
		for(unsigned int i = 0; i < _despawnPending.size(); ++i)
		{
			DespawnSceneObject(_despawnPending[i]);
		}
		_despawnPending.clear();

		for(unsigned int i = 0; i < _destroyPending.size(); ++i)
		{
			DestroySceneObject(_destroyPending[i]);
		}
		_destroyPending.clear();

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
			if( _players->GetPlayer(owner)->IsUsed() )
			{
				string defName = objNode->first_attribute("name",4,false)->value();
				string defType = objNode->first_attribute("type",4,false)->value();
				// Pair spawn info with SODefinition of object to be spawned
				_mapInitObjects.push_back(std::make_pair(
					std::make_pair(spawnInfo, owner),
					static_cast<SceneObjectDefinition*>(
					_objectPool->GetObjectDefinitionByName(defName,defType))));
			}
			// Get next SceneObject
			objNode = objNode->next_sibling("SceneObject");
		}
	}

	void GameWorld::LoadTerrain()
	{
		XmlNode* mapNode = _gameSettings->MapFile->first_node("Map",3);

		RefPtr<MapTerrain> terrain(xNew3(MapTerrain, _ogreSceneMgr, 
			_map->GetCellSize().x, 
			_map->GetTileSize().X));
		terrain->ReadTerrainInfo(mapNode);
		_map->SetTerrain(terrain);
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

		_minimap = xNew4(Minimap, _map->GetTerrainSize(), 
			IntVector2(512, (int)(512.f * (_map->GetTerrainSize().y / _map->GetTerrainSize().x))),
			mapPreiviewImageName, "Maps");

		_minimap->SetUseTerrainTexture();
	}

	void GameWorld::CreatePathingSystem()
	{
		auto& terrInfo = _map->GetTerrain()->GetTerrainInfo();
		_pathing = xNew4(PathingSystem,
			_ogreSceneMgr,
			terrInfo.GetTilesX()*terrInfo.GetTileCellSize(), 
			terrInfo.GetTilesZ()*terrInfo.GetTileCellSize(),
			_map->GetCellSize());
		_pathing->SetRootDebugNode(_ogreSceneMgr->getRootSceneNode());
	}

	void GameWorld::CreateWorld()
	{
		// Load actual terrain
		_map->CreateTerrain();

		CreateMinimap();

		// As terrains are loaded, may create terrain obstacles
		_pathing->CreateSlopeObstacles(_map->GetTerrain().GetRawPtr());

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
			SceneObject* obj = CreateSceneObject(objIt->second, objIt->first.second);
			SpawnSceneObject(obj, *objIt->first.first);
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

	SceneObject* GameWorld::CreateSceneObject(SceneObjectDefinition* objectDef, int owner)
	{
		SceneObject* object = 
			static_cast<SceneObject*>(_objectPool->Create(objectDef, owner));
		object->LoadResources(_ogreSceneMgr);
		object->ChangeState(SceneObjectStates::NotSpawned);
		return object;
	}

	SceneObject* GameWorld::CreateSceneObject(ObjectHandle objectDefHandle, int owner)
	{
		ObjectDefinition* objectDef = _objectPool->GetObjectDefinitionByHandle(objectDefHandle);
		return CreateSceneObject(static_cast<SceneObjectDefinition*>(objectDef), owner);
	}

	/*Unit* GameWorld::CreateUnit(UnitDefinition* objectDef, uint8 owner)
	{
		auto unitsPool = static_cast<UnitPool*>(_objectPool->GetObjectPool(GetTypeId<Unit>()));
		Unit* object = unitsPool->CreateUnit(objectDef, owner);
		object->LoadResources(_ogreSceneMgr);
		object->ChangeState(SceneObjectStates::NotSpawned);
		return object;
	}

	Unit* GameWorld::CreateUnit(ObjectHandle objectDefHandle, uint8 owner)
	{
		ObjectDefinition* objectDef = _objectPool->GetObjectDefinitionByHandle(objectDefHandle);
		return CreateUnit(static_cast<UnitDefinition*>(objectDef), owner);
	}*/

	void GameWorld::DestroySceneObject(SceneObject* object)
	{
		// DespawnObject(object)
		object->UnloadResources(_ogreSceneMgr);
		_objectPool->Destroy(object);
	}

	void GameWorld::DestroySceneObject(ObjectHandle objectHandle)
	{
		IGameObject* baseObject = reinterpret_cast<IGameObject*>(objectHandle.Object);
		DestroySceneObject( static_cast<SceneObject*>(baseObject) );
	}

	void GameWorld::QueueDestroySceneObject(SceneObject* object)
	{
		_destroyPending.push_back(object);
	}

	void GameWorld::SpawnSceneObject(SceneObject* object, 
		const SpawnInfo& spawnInfo)
	{
		_map->SpawnObject(object, spawnInfo);
		if(object->HavePhysics())
		{
			if(object->HaveFootprint())
			{
				_pathing->AddObstacle(static_cast<PhysicalObject*>(object));
			}
			_pathing->AddPathingObject(static_cast<PhysicalObject*>(object));
		}

		int mmapFlags = object->GetMinimapFlags();
		if( mmapFlags != MinimapFlags::None &&
			(mmapFlags & MinimapFlags::NeverVisible) == 0 )
		{
			// Object is to be shown on minimap, so add it to minimap
			// But first set it owner flag
			if( object->GetOwner() == NEUTRAL_PLAYERNUM )
				mmapFlags |= MinimapFlags::EnvironmentUnit;
			else if( _players->IsOwned(object->GetOwner()) )
				mmapFlags |= MinimapFlags::OwnedUnit;
			else if( _players->IsAlly(_players->GetClientPlayer(), object->GetOwner()) )
				mmapFlags |= MinimapFlags::AllyUnit;
			else if( _players->IsNeutral(_players->GetClientPlayer(), object->GetOwner()) )
				mmapFlags |= MinimapFlags::NeutralUnit;
			else if( _players->IsEnemy(_players->GetClientPlayer(), object->GetOwner()) )
				mmapFlags |= MinimapFlags::EnemyUnit;

			object->SetMinimapFlags(mmapFlags);

			_minimap->AddObject(object);
		}

		if(object->GetFinalType() == GetTypeId<Unit>() &&
			static_cast<Unit*>(object)->CountsAsPlayerUnit() &&
			static_cast<Unit*>(object)->GetOwner() < 8u) // If owner >= 8 it belongs to neutral player ( environment ), so don't add it to stats
		{
			_statsManager->UnitSpawned(  static_cast<Unit*>(object) );
		}
	}

	void GameWorld::SpawnSceneObject(ObjectHandle objectHandle, 
		const SpawnInfo& spawnInfo)
	{
		IGameObject* baseObject = reinterpret_cast<IGameObject*>(objectHandle.Object);
		SpawnSceneObject(static_cast<SceneObject*>(baseObject), spawnInfo);
	}

	void GameWorld::DespawnSceneObject(SceneObject* object)
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
	}

	void GameWorld::DespawnSceneObject(ObjectHandle objectHandle)
	{
		IGameObject* baseObject = reinterpret_cast<IGameObject*>(objectHandle.Object);
		DespawnSceneObject(static_cast<SceneObject*>(baseObject));
	}

	void GameWorld::QueueDespawnSceneObject(SceneObject* object)
	{
		_despawnPending.push_back(object);
	}

	SceneObjectDefinition* GameWorld::GetSceneObjectDefinition(ObjectHandle defHandle)
	{
		return static_cast<SceneObjectDefinition*>(
			_objectPool->GetObjectDefinitionByHandle(defHandle));
	}

	SceneObjectDefinition* GameWorld::GetSceneObjectDefinition(
		const string& objectFamilyName, 
		const string& objectKindName)
	{
		return static_cast<SceneObjectDefinition*>(
			_objectPool->GetObjectDefinitionByName(objectKindName, objectFamilyName));
	}

	IGameObject* GameWorld::CreateGameObject(ObjectDefinition* objectDef)
	{
		return _objectPool->Create(objectDef, NEUTRAL_PLAYERNUM);
	}

	IGameObject* GameWorld::CreateGameObject(ObjectHandle objectDefHandle)
	{
		ObjectDefinition* objectDef = _objectPool->GetObjectDefinitionByHandle(objectDefHandle);
		return _objectPool->Create(objectDef, NEUTRAL_PLAYERNUM);
	}

	void GameWorld::DestroyGameObject(IGameObject* object)
	{
		_objectPool->Destroy(object);
	}

	void GameWorld::DestroyGameObject(ObjectHandle objectHandle)
	{
		IGameObject* baseObject = reinterpret_cast<IGameObject*>(objectHandle.Object);
		_objectPool->Destroy(baseObject);
	}

	ObjectDefinition* GameWorld::GetGameObjectDefinition(ObjectHandle defHandle)
	{
		return _objectPool->GetObjectDefinitionByHandle(defHandle);
	}

	ObjectDefinition* GameWorld::GetGameObjectDefinition(
		const string& objectFamilyName, 
		const string& objectKindName)
	{
		return _objectPool->GetObjectDefinitionByName(objectKindName, objectFamilyName);
	}

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
		for(unsigned int dataSup = 0; dataSup < _registeredSups->KindDataSups.size(); ++dataSup)
		{
			IKindSpecificDataSupplier* kdsup =	_registeredSups->KindDataSups[dataSup];
			IObjectSpecificDataSupplier* odsup = _registeredSups->ObjectDataSups[dataSup];
			string name = _registeredSups->DataSupsNames[dataSup];
			auto setKindData = [ this, name, kdsup, odsup ]() 
			{
				// For each definition check if it have given ObjectData name
				auto allDefs = _objectPool->GetDefinitionsCreator()->GetAllDefinitions();
				for(unsigned int i = 0; i < allDefs->size(); ++i)
				{
					auto& def = (*allDefs)[i];
					if( def->GetObjectDataName().compare(name) == 0 )
					{
						def->SetKindDataHandleSupplier(kdsup);
						def->SetObjectDataHandleSupplier(odsup);
						def->SetKindSpecificDataHandle(kdsup->GetKindSpecificDataHandle());
					}
				}
			};

			_objectPool->OnAllDefinitionsLoaded() +=
				xNew1(DelegateEventHandler<decltype(setKindData)>, setKindData);
		}

		UnitPool* unitPool = (UnitPool*)_objectPool->GetObjectPool(GetTypeId<Unit>());

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
		}
	}

	void GameWorld::DeactivateAllRegisteredSuppliers()
	{
		UnitPool* unitPool = (UnitPool*)_objectPool->GetObjectPool(GetTypeId<Unit>());

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
		_registeredSups->ClassSupsNames.clear();
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

	void GameWorld::ShowQuickNotice(const NoticeMessage& msg)
	{
		_quickNoticeRequested.Fire(msg);
	}

	void GameWorld::ShowNotice(const NoticeMessage& msg)
	{

		_noticeRequested.Fire(msg);
	}
}