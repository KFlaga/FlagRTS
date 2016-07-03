#pragma once

#include "DataTypes.h"
#include "SpawnInfo.h"
#include <Event.h>
#include <Array.h>
#include <ObjectHandle.h>
#define NEUTRAL_PLAYERNUM 8

namespace Ogre
{
	class SceneManager;
	class RenderWindow;
}

namespace FlagRTS
{
	class MainGameObjectPool;
	class Map;
	class SceneObject;
	class SceneObjectDefinition;
	class ObjectDefinition;
	class IGameObject;
	class PathingSystem;
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
	struct NoticeMessage;
	class Minimap;
	struct InGameSettings;


	// Class that loads, creates and stores actual map with terrain/objects etc
	// It also delegates spawn/despawn/load/unload of game object
	// Also stores all 'global' classes (like map, players, pathing) needed by other objects, so
	// they should take them from GameWorld::GlobalWorld
	class GameWorld
	{
	public:
		static GameWorld* GlobalWorld;
		typedef Array<std::pair<std::pair<SpawnInfo*, int>, SceneObjectDefinition*>> SpawnList;

	private:
		MainGameObjectPool* _objectPool; // Not owned by GameWorld
		Ogre::SceneManager* _ogreSceneMgr;
		Ogre::RenderWindow* _gameWindow;
		InGameSettings* _gameSettings;
		Map* _map; // Map created in and owned by GameWorld
		GlobalStatisticsManager* _statsManager; // Not owned by GameWorld ( may be useful after game ends - i.e. score screen )
		FactionsManager* _factionsManager;  // Factions created and owned by GameWorld
		GlobalTechnologyManager* _techsManager; // Created and owned by GameWorld
		PathingSystem* _pathing; // Created and owned by GameWorld
		ConstructionManager* _constructionMgr; // Created and owned by GameWorld
		// List of objects to be spawned on map inital state 
		SpawnList _mapInitObjects;
		SuppliersData* _registeredSups;
		Minimap* _minimap;

		PlayersInfo* _players; // 
		Resources* _resources;

		Array<SceneObject*> _despawnPending;
		Array<SceneObject*> _destroyPending;

	public:
		GameWorld(Ogre::SceneManager* sceneMgr,
			Ogre::RenderWindow* gamewindow,
			MainGameObjectPool* objectPool,
			GlobalStatisticsManager* statsManager
			);
		~GameWorld();

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

		SceneObject* CreateSceneObject(SceneObjectDefinition* objectDef, int owner = NEUTRAL_PLAYERNUM);
		SceneObject* CreateSceneObject(ObjectHandle objectDefHandle, int owner = NEUTRAL_PLAYERNUM);
		//Unit* CreateUnit(UnitDefinition* objectDef, uint8 owner);
		//Unit* CreateUnit(ObjectHandle objectDefHandle, uint8 owner);
		void DestroySceneObject(SceneObject* object);
		void QueueDestroySceneObject(SceneObject* object);
		void DestroySceneObject(ObjectHandle objectHandle);
		void SpawnSceneObject(SceneObject* object, 
			const SpawnInfo& spawnInfo);
		void SpawnSceneObject(ObjectHandle objectHandle, 
			const SpawnInfo& spawnInfo);
		void DespawnSceneObject(SceneObject* object);
		void QueueDespawnSceneObject(SceneObject* object);
		void DespawnSceneObject(ObjectHandle objectHandle);
		SceneObjectDefinition* GetSceneObjectDefinition(ObjectHandle defHandle);
		SceneObjectDefinition* GetSceneObjectDefinition(
			const string& objectFamilyName, 
			const string& objectKindName);

		// Creates game object other than SceneObject
		IGameObject* CreateGameObject(ObjectDefinition* objectDef);
		// Creates game object other than SceneObject
		IGameObject* CreateGameObject(ObjectHandle objectDefHandle);
		// Destroys game object other than SceneObject
		void DestroyGameObject(IGameObject* object);
		// Destroys game object other than SceneObject
		void DestroyGameObject(ObjectHandle objectHandle);
		ObjectDefinition* GetGameObjectDefinition(ObjectHandle defHandle);
		ObjectDefinition* GetGameObjectDefinition(
			const string& objectFamilyName, 
			const string& objectKindName);


		Ogre::SceneManager* GetOgreSceneManager() const { return _ogreSceneMgr; }
		// Returns prevoiusly created pathing system
		PathingSystem* GetPathingSystem() const { return _pathing; }

		PlayersInfo* GetPlayers() const { return _players; }

		Resources* GetResources() const { return _resources; }
		void SetNewResourcesData(Resources* resources);

		MainGameObjectPool* GetObjectPool() const { return _objectPool; }

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

	private:
		Event<const NoticeMessage&> _noticeRequested;
		Event<const NoticeMessage&> _quickNoticeRequested;

	public:
		// Shows short notice on screen ( quick fade, best for info on events that need short, fast notice,
		// like 'insufficient resources', 'invalid location', 'inaccessible action' etc )
		void ShowQuickNotice(const NoticeMessage& msg);

		// Shows short notice on screen (remains few seconds)
		void ShowNotice(const NoticeMessage& msg);

		Event<const NoticeMessage&>& NoticeRequested() { return _noticeRequested; }
		Event<const NoticeMessage&>& QuickNoticeRequested() { return _quickNoticeRequested; }
	};
}