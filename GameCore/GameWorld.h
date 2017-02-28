#pragma once

#include "DataTypes.h"
#include <Event.h>
#include <Array.h>
#include <ObjectHandle.h>

namespace FlagRTS
{
	//template<typename T>
	//class IDataStorage;

	class IObjectDefinitionManager;
	class IGameObjectPoolManager;
	class IGameObjectPool;
	class ObjectDefinition;
	class ISceneObjectSpawner;
	class IPathingSystem;
	class Map;

	class IGameObject;
	class SpawnInfo;
	class SceneObject;
	class SceneObjectDefinition;
	class UnitDefinition;
	class Unit;
	class IKindSpecificDataSupplier;
	class IUnitCommandSupplier;
	class IObjectStateSupplier;
	class IObjectSpecificDataSupplier;
	class UnitClassSupplier;
	struct SuppliersData;
	class PlayersInfo;
	class Resources;
	class ConstructionManager;
	class GlobalStatisticsManager;
	class GlobalTechnologyManager;
	class FactionsManager;
	class Minimap;
	struct InGameSettings;

	struct NoticeMessage;
	class INoticeMessageSender;

	// Class that loads, creates and stores actual map with terrain/objects etc
	// It also delegates spawn/despawn/load/unload of game object
	// Also stores all 'global' classes (like map, players, pathing) needed by other objects, so
	// they should take them from GameWorld::GlobalWorld
	class GameWorld
	{
		DISALLOW_COPY(GameWorld);

	public:
		static GameWorld* GlobalWorld;

		typedef Array<std::pair<SpawnInfo*, SceneObjectDefinition*>> SpawnList;

	protected: // System interfaces 
		IGameObjectPoolManager* _objectPoolManager; // NOT owned by GameWorld
		IGameObjectPool* _mainObjectPool; // NOT owned by GameWorld
		IObjectDefinitionManager* _objectDefinitionManager; // NOT owned by GameWorld
		ISceneObjectSpawner* _sceneObjectSpawner; // NOT owned by GameWorld
		INoticeMessageSender* _noticeSender; // NOT owned by GameWorld

		// IDataStorage<size_t>* _objectDataStorage; // owned by GameWorld
	protected:
		Ogre::SceneManager* _ogreSceneMgr;
		InGameSettings* _gameSettings;
		Map* _map; // Map created in and owned by GameWorld
		GlobalStatisticsManager* _statsManager; // Not owned by GameWorld ( may be useful after game ends - i.e. score screen )
		FactionsManager* _factionsManager;  // Factions created and owned by GameWorld
		GlobalTechnologyManager* _techsManager; // Created and owned by GameWorld
		IPathingSystem* _pathing; // Created and owned by GameWorld
		ConstructionManager* _constructionMgr; // Created and owned by GameWorld
		// List of objects to be spawned on map inital state 
		SpawnList _mapInitObjects;
		SuppliersData* _registeredSups;
		Minimap* _minimap;

		PlayersInfo* _players; // 
		Resources* _resources;

	protected:
		GameWorld();

	public:
		// If any of needed interfaces is 0, default implementation is created
		GameWorld(Ogre::SceneManager* sceneMgr,
			IObjectDefinitionManager* objectDefinitionManager,
			IGameObjectPoolManager* objectPoolManager,
			IGameObjectPool* mainObjectPool,
			ISceneObjectSpawner* sceneObjSpawner,
			INoticeMessageSender* noticeSender,
			GlobalStatisticsManager* statsManager
			);
		~GameWorld();

		/// Updates main game components
		/**
		Updates:
		1) Main object pool
		2) PathingSystem
		3) SceneObjectSpawner
		4) Minimap
		*/
		void Update(float ms);

		// Sets used map settings
		void SetGameSettings(InGameSettings* settings);

		// Loads terrain from set map
		void LoadTerrain();
		// Creates all Managers / PathingSystem / Minimap ( game settings must be set )
		// (also sets player info)
		void CreateGameManagers();
		// Saves spawn infos for set map
		void LoadSpawnInfos();

		// Creates map - all scene objects etc and attaches it to
		// Map object ( reason for disjuntive load and create is that
		// it allows quicker map restart/load (resources are already loaded))
		void CreateWorld();

		// Shows map on screen, attaches all events listeners ( or something like that )
		void ShowWorld();
		// Hides map from screen, removes all events listeners ( or something like that )
		void HideWorld();

		// Resets map back to its initial state ( that is after load 
		// but before create -> deletes all objects and resets players stats/technology )
		void ResetWorld();

		// Destroys all created objects, unload terrain, destroys Managers
		void UnloadMap();

		Map* GetMap() const { return _map; }

		Ogre::SceneManager* GetOgreSceneManager() const { return _ogreSceneMgr; }

		PlayersInfo* GetPlayers() const { return _players; }

		Resources* GetResources() const { return _resources; }
		void SetNewResourcesData(Resources* resources);

	public: // System interfaces
		IGameObjectPoolManager* GetGameObjectPoolManager() const { return _objectPoolManager; }
		IGameObjectPool* GetGameObjectPool() const { return _mainObjectPool; }
		IObjectDefinitionManager* GetObjectDefinitionManager() const { return _objectDefinitionManager; }
		ISceneObjectSpawner* GetSceneObjectSpawner() const { return _sceneObjectSpawner; } 
		IPathingSystem* GetPathingSystem() const { return _pathing; }

		//IDataStorage<size_t>* GetObjectDataStorage() const { return _objectDataStorage; }
		INoticeMessageSender* GetNoticeMessageSender() const { return _noticeSender; }

		ConstructionManager* GetConstructionManager() const { return _constructionMgr; }

		GlobalStatisticsManager* GetPlayerStatsManager() const { return _statsManager; }

		FactionsManager* GetFactionsManager() const { return _factionsManager; }

		GlobalTechnologyManager* GetTechnologyManager() const { return _techsManager; }

		Minimap* GetMinimap() const { return _minimap; }

	protected:
		void CreateMinimap();	
		void CreatePathingSystem();

	public:
		void ActivateAllRegisteredSuppliers();
		void DeactivateAllRegisteredSuppliers();

		void RegisterDataSupplier(const string& name,
			IKindSpecificDataSupplier* kdsupplier, 
			IObjectSpecificDataSupplier* odsupplier);

		void RegisterObjectStatesSupplier(const string& name, 
			size_t targetObjectsTypeId, IObjectStateSupplier* supplier);

		void RegisterUnitCommandsSupplier(const string& name, IUnitCommandSupplier* supplier);
		void RegisterUnitClassSupplier(const string& name, UnitClassSupplier* supplier);
	};

	/// Provides global access to game system interfaces
	/**
	Static class that provides simple global access to important game world components' interfaces.
	All interfaces are querried via global GameWorld ('GameWorld::GlobalWorld').
	*/
	class GameInterfaces
	{
		DISALLOW_COPY(GameInterfaces);
		GameInterfaces();

	public:
		static GameWorld* GetGameWorld() { return GameWorld::GlobalWorld; }

		static IGameObjectPoolManager* GetGameObjectPoolManager() { return GetGameWorld()->GetGameObjectPoolManager(); }
		static IGameObjectPool* GetGameObjectPool() { return GetGameWorld()->GetGameObjectPool(); }
		static IObjectDefinitionManager* GetObjectDefinitionManager() { return GetGameWorld()->GetObjectDefinitionManager(); }
		static ISceneObjectSpawner* GetSceneObjectSpawner(){ return GetGameWorld()->GetSceneObjectSpawner(); }
		//static IDataStorage<size_t>* GetObjectDataStorage() { return GetGameWorld()->GetObjectDataStorage(); }
		static INoticeMessageSender* GetNoticeMessageSender() { return GetGameWorld()->GetNoticeMessageSender(); }
		static IPathingSystem* GetPathingSystem() { return GetGameWorld()->GetPathingSystem(); }
	
	};
}