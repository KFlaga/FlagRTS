#pragma once

#include "IPathingSystem.h"
#include <Event.h>
#include <Xml.h>
#include <ArrayMap.h>
#include "CollisionShapes.h"
#include "PathingComponent.h"

namespace Ogre
{
	class SceneManager;
	class SceneNode;
}

namespace FlagRTS
{
	class UniformGridPathingMap;
	class IGlobalPathFinder;
	template<typename UnitT>
	class ILocalPathFinder;
	class UniformGridObstacle;
	template<typename UnitT>
	class Dbvh;

	class PathingDrawer;
	class PhysicalObject;
	class Unit;
	class SceneObject;
	class PathRequestHandler;
	struct IBoxPathingQuery;
	class TerrainBase;

	class PathingSystem : public IPathingSystem
	{
	private:
		Ogre::SceneManager* _ogreMgr;
		Ogre::SceneNode* _debugNode;
		UniformGridPathingMap* _pathingMap;
		IGlobalPathFinder* _globalFinder;
		ILocalPathFinder<PathingComponent>* _localFinder;
		Dbvh<BoundingBox>* _dbvh;
		size_t _globalFinderThreadHandle;
		PathRequestHandler* _pathRequestHandler;
		Array<PhysicalObject*> _lastCollisions;

		Array<PathingDrawer*> _debugDrawers;
		Array<bool> _useDrawer;
		
		PhysicalObject* _initDbvhObject;
		float _goalTolerance;
		float _fixedInterval;

	public:
		PathingSystem(Ogre::SceneManager* ogreMgr,
			const int cellCountX, 
			const int cellCountY,
			const float cellSize);
		~PathingSystem();

		// Creates obstacles on map, based on terrain height differences 
		// and rules read from PathingConfig.
		// Must be called after terrains are loaded
		void CreateSlopeObstacles(TerrainBase* terrain);

		void Update(float ms);

		void SetRootDebugNode(Ogre::SceneNode* debugnode);

		// Adds obstacle to pathing system
		void AddObstacle(PhysicalObject* object);
		// Removes obstacle from pathing system
		void RemoveObstacle(PhysicalObject* object);

		// Adds object to pathing system and sets its pathing handle
		void AddPathingObject(PhysicalObject* unit);
		// Removes unit woth given handle from pathing system
		// Handle is invalid after this call
		void RemovePathingObject(PhysicalObject* unit);

		// Set units goal on map
		// If 'moveToGoal' = true, local pathfinder will try to find frame-moves for unit
		// ( and so ShouldUnitMove() should return true )
		// If 'requestPath' = true, global pathfinder will try to find optimal path to goal cell
		// ( same as calling RequestPathToGoal() later )
		// If both flags are set, then unit will start moving localy to goal, then
		// when global path is available it will switch to it
		void SetUnitGlobalGoal(Unit* unit, const Vector3& goal, 
			bool moveToGoal = false, bool requestPath = false);

		void RequestPath(PhysicalObject* object);
		void AbandonPath(PhysicalObject* object);

		// Returns true if unit should move in this frame
		bool ShouldUnitMove(Unit* unit);

		// Returns next point unit should move to
		Vector2 GetUnitNextMove(Unit* unit);

		// Sets wether unit can be moved
		void SetUnitHoldPosition(Unit* unit, bool holdPos);

		UniformGridPathingMap* GetPathingMap() const { return _pathingMap; }
		ILocalPathFinder<PathingComponent>* GetLocalFinder() const { return _localFinder; }
		IGlobalPathFinder* GetGlobalFinder() const { return _globalFinder; }

		IPathingQuery* CreatePathingQuery();

		float GetDistanceBetweenObjects(PhysicalObject* obj1, PhysicalObject* obj2);

		// Returns array with objects that collides with given object in 3d
		// Results are valid untill next call
		Array<PhysicalObject*>& Find3dCollisions(PhysicalObject* collider);

	private:
		void CreatePathingMap(XmlNode* rootNode, 
			int x, int y, float cellSize);
		void InitGlobalPathFinder(XmlNode* rootNode);
		void InitLocalPathFinder(XmlNode* rootNode);
		void ProcessFinishedPathRequests();

		void OnUnitMoved(SceneObject* unit);
		DEFINE_DELEGATE1(UnitMovedDelegate, PathingSystem,
			OnUnitMoved, SceneObject*);
		UnitMovedDelegate _onUnitMoved;

		void OnUnitRotated(SceneObject* unit);
		DEFINE_DELEGATE1(UnitRotateDelegate, PathingSystem,
			OnUnitRotated, SceneObject*);
		UnitRotateDelegate _onUnitRotated;
	};
}