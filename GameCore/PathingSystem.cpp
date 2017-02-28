#include "PathingSystem.h"
#include "PathingDrawer.h"

#include <InputManager.h>
#include <KeyBindings.h>

#include "PathingUniformGridMap.h"
#include "PathingJumpGlobalPathFinder.h"
#include "PathingVariedSizeJumpGlobalPathFinder.h"
#include "PathingForcesLocalPathFinder.h"
#include "PathingIgnoreUnitsLocalPathFinder.h"
#include "PathingDbvh.h"
#include "PathingQuery.h"
#include "PathingDbvhUUPairer.h"

#include <OgreResourceGroupManager.h>
#include "Unit.h"
#include "GameWorld.h"
#include <Thread.h>
#include <ThreadManager.h>
#include "PathRequestHandler.h"

#include "CollisionFilter.h"
#include "TerrainBase.h"
#include "SlopeObstacles.h"

#include "CollisionComponent.h"
#include "PathingComponent.h"

namespace FlagRTS
{
	using namespace PathFinding;

	enum DebugDrawersNum : int
	{
		GridDrawer = 0,
		DbvhDrawer = 1,
		GroundObstaclesDrawer = 2,
		BuildingObstaclesDrawer = 3,
		DebugDrawersCount,
	};

	// TODO Change to common (multi-filter) result database
	class JumpPointFinder : public IGlobalPathFinder
	{
	public:
		enum FinderType
		{
			NormalJP,
			VariedSizeJP
		};

	private:
		std::map<int, IGlobalPathFinder*> _perFilterPathfinders;
		UniformGridPathingMap* _pathingMap;
		float _eps;
		FinderType _finderType;

	public:
		JumpPointFinder(UniformGridPathingMap* pmap, FinderType finderType) :
			_pathingMap(pmap),
			_finderType(finderType)
		{ }

		~JumpPointFinder()
		{
			for(auto pfIt = _perFilterPathfinders.begin(); pfIt != _perFilterPathfinders.end(); ++pfIt)
				xDelete(pfIt->second);
		}

		void SetHeuristicMultiplier(const float eps) { _eps = eps; }

		void FindPath(const IntVector2& beginCell, 
			const IntVector2& endCell)
		{
			IGlobalPathFinder* finder;
			auto finderIt = _perFilterPathfinders.find(_filter.Groups);
			if(finderIt != _perFilterPathfinders.end())
			{
				finder = finderIt->second;
			}
			else
			{
				if( _finderType == VariedSizeJP )
				{
					finder = xNew1(UniformGridVariedSizeJumpFinder, _pathingMap);
					((UniformGridVariedSizeJumpFinder*)finder)->SetHeuristicMultiplier(_eps);
				}
				else
				{
					finder = xNew1(UniformGridJumpPointFinder, _pathingMap);
					((UniformGridJumpPointFinder*)finder)->SetHeuristicMultiplier(_eps);
				}
				_perFilterPathfinders.insert(std::make_pair(_filter.Groups, finder));
				finder->SetBlockFilter(_filter);
				finder->UpdateAll();
			}
			finder->SetUnitSize(_unitSize);
			finder->FindPath(beginCell, endCell);
			_lastPath = finder->GetLastPath();
			_lastPathReal = finder->GetLastPathReal();
		}


		void UpdateAll()
		{
			for(auto pfIt = _perFilterPathfinders.begin(); pfIt != _perFilterPathfinders.end(); ++pfIt)
				pfIt->second->UpdateAll();
		}

		void UpdateCell(int x, int y)
		{
			for(auto pfIt = _perFilterPathfinders.begin(); pfIt != _perFilterPathfinders.end(); ++pfIt)
				pfIt->second->UpdateCell(x,y);
		}

		void UpdateObstacle(const Array2d<CollisionFilter>& obstacle, const IntVector2& topLeftCell)
		{
			for(auto pfIt = _perFilterPathfinders.begin(); pfIt != _perFilterPathfinders.end(); ++pfIt)
				pfIt->second->UpdateObstacle(obstacle, topLeftCell);
		}
	};

	struct PathingComponentGetter
	{
		PathingComponent* operator()(PhysicalObject* obj)
		{
			return obj->FindComponent<PathingComponent>();
		}
	};

	PathingSystem::PathingSystem(
		Ogre::SceneManager* ogreMgr,
		const int cellCountX, 
		const int cellCountY,
		const float cellSize) :
	_ogreMgr(ogreMgr),
		_debugNode(0),
		_localFinder(0),
		_globalFinder(0),
		_onUnitMoved(this),
		_onUnitRotated(this),
		_pathRequestHandler(0),
		_initDbvhObject(0)
	{
		// Read config:
		RefPtr<XmlDocument> pathingConfig = XmlUtility::XmlDocFromOgreResource("PathingConfig.xml", "Base");
		XmlNode* rootNode = pathingConfig->first_node("PathingConfig");

		CreatePathingMap(rootNode, cellCountX, cellCountY, cellSize);

		_debugDrawers.resize(DebugDrawersCount, 0);
		_useDrawer.resize(DebugDrawersCount, false);

		_debugDrawers[GridDrawer] = xNew3(PathingDrawer,ogreMgr, Vector3(0.0f,0.0f,1.0f), "GridDrawer");
		_debugDrawers[GroundObstaclesDrawer] = xNew3(PathingDrawer,ogreMgr, Vector3(0.2f,0.2f,0.5f), "GObstacleDrawer");
		_debugDrawers[DbvhDrawer] = xNew3(PathingDrawer,ogreMgr, Vector3(0.6f,0.1f,0.1f), "DbvhDrawer");
		_debugDrawers[BuildingObstaclesDrawer] = xNew3(PathingDrawer,ogreMgr, Vector3(0.5f,0.2f,0.2f), "BObstacleDrawer");

		_dbvh = xNew2(Dbvh<BoundingBox>, 0.9f, 0.75f);
		InitGlobalPathFinder(rootNode);
		InitLocalPathFinder(rootNode);

		_goalTolerance = XmlUtility::XmlGetFloat(rootNode->first_node("PathGoalTolerance"), "value");
		// _isToleranceRelative = XmlUtility::XmlGetBool(rootNode->first_node("PathGoalToleranceRelative"), "value");
	}

	void PathingSystem::CreatePathingMap(XmlNode* rootNode, 
		int x, int y, float cellSize)
	{
		XmlNode* mapNode = rootNode->first_node("MapRepresentation");

		CollisionFilter filter = 0;
		XmlNode* pathingGroupsNode = mapNode->first_node("DefaultGroups");
		if(pathingGroupsNode != 0)
		{
			XmlNode* groupNode =  pathingGroupsNode->first_node();
			while(groupNode != 0)
			{
				filter.Groups |= CollisionGroups::ParseCollisionGroupType(
					XmlUtility::XmlGetString(groupNode, "value"));
				groupNode = groupNode->next_sibling();
			}
		}

		pathingGroupsNode = mapNode->first_node("DefaultBlockedGroups");
		if(pathingGroupsNode != 0)
		{
			XmlNode* groupNode =  pathingGroupsNode->first_node("Group");
			while(groupNode != 0)
			{
				filter.Blocking |= CollisionGroups::ParseCollisionGroupType(
					XmlUtility::XmlGetString(groupNode, "value"));
				groupNode = groupNode->next_sibling();
			}
		}

		_pathingMap = xNew4(UniformGridPathingMap,
			x, y, Vector2(cellSize, cellSize), filter);
	}

	void PathingSystem::CreateSlopeObstacles(TerrainBase* terrain)
	{
		RefPtr<XmlDocument> pathingConfig = XmlUtility::XmlDocFromOgreResource("PathingConfig.xml", "Base");
		XmlNode* rootNode = pathingConfig->first_node("PathingConfig");

		HeightMapConverter heightMapConv;
		heightMapConv.SetMap(_pathingMap, &terrain->GetFinalHeightMap());

		// Read slope rules
		XmlNode* rulesListNode = rootNode->first_node("SlopeRules");
		if(rulesListNode != 0)
		{
			XmlNode* ruleNode = rulesListNode->first_node();
			while(ruleNode != 0)
			{
				SlopeRule rule;
				rule.Filter.Blocking |= CollisionGroups::ParseCollisionGroupType(
					XmlUtility::XmlGetString(ruleNode->first_node("Blocks"), "value"));
				rule.MinAngle = XmlUtility::XmlGetFloat(ruleNode->first_node("MinAngle"), "value");
				rule.MaxAngle = XmlUtility::XmlGetFloat(ruleNode->first_node("MaxAngle"), "value");
				heightMapConv.AddSlopeRule(rule);

				ruleNode = ruleNode->next_sibling();
			}
		}

		heightMapConv.ComputeObstacles();
		_globalFinder->UpdateAll();
	}

	void PathingSystem::InitGlobalPathFinder(XmlNode* rootNode)
	{
		XmlNode* gPFNode = rootNode->first_node("GlobalPathFinder");
		string gpfName =  XmlUtility::XmlGetStringIfExists(gPFNode, "name", "JumpPoint");
		JumpPointFinder* gpf = 0;

		if(gpfName.compare("VariedSizeJumpPoint") == 0)
		{
			gpf = xNew2(JumpPointFinder, _pathingMap, JumpPointFinder::VariedSizeJP);
		}
		else
		{
			gpf = xNew2(JumpPointFinder, _pathingMap, JumpPointFinder::NormalJP);
		}
		float eps = XmlUtility::XmlGetFloat(
			gPFNode->first_node("HeuristicMultiplier"), "value");
		gpf->SetHeuristicMultiplier(eps);
		_globalFinder = gpf;

		_pathRequestHandler = xNew1(PathRequestHandler, _globalFinder);

		Thread* gpfThread = ThreadManager::Instance()->CreateThread("GlobalPathfinderThread");
		if(!Thread::IsMainThread())
		{
			Thread* currThread = ThreadManager::Instance()->GetCurrentThread();
			currThread->AddChildThread(gpfThread);
		}
		_globalFinderThreadHandle = gpfThread->GetHandle();
		gpfThread->Run(ProcessGlobalPathRequests, _pathRequestHandler);
	}

	void PathingSystem::InitLocalPathFinder(XmlNode* rootNode)
	{
		XmlNode* lpfNode = rootNode->first_node("LocalPathFinder");
		string lpfName =  XmlUtility::XmlGetString(lpfNode, "name", 4);
		if(lpfName.compare("Forces") == 0)
		{
			auto uuPairer = new DbvhUnitUnitLookupPairer2d<BoundingBox,
				PathingComponent,
				FindUnitsWithinLookupShapeQuery2d<BoundingBox,
				PhysicalObject, PathingComponent, 
				PathingComponentGetter>>(_dbvh);
			ForcesPathFinder<PathingComponent>* lpf = xNew2(
				ForcesPathFinder<PathingComponent>, _pathingMap, uuPairer);
			uuPairer->SetUnitSource( lpf->GetUnits() );

			float repCoeff = XmlUtility::XmlGetFloat(
				lpfNode->first_node("RepulsionCoeff"), "value");
			float attCoeff = XmlUtility::XmlGetFloat(
				lpfNode->first_node("AttractionCoeff"), "value");
			float repFalloff = XmlUtility::XmlGetFloat(
				lpfNode->first_node("RespulsionFalloff"), "value");
			bool scaleFalloff = XmlUtility::XmlGetBool(
				lpfNode->first_node("ScaleRepulsionFalloff"), "value");
			lpf->SetGoalAttractionForceCoeff(attCoeff);
			lpf->SetIsRepulsionFallOffScaling(scaleFalloff);
			lpf->SetRepulsionForceCoeff(repCoeff);
			lpf->SetRepulsionForceFallOff(repFalloff);
			_localFinder = lpf;
		}
		else if(lpfName.compare("IgnoreUnits") == 0)
		{
			UnitIgnorePathFinder<PathingComponent>* lpf = xNew1(
				UnitIgnorePathFinder<PathingComponent>, _pathingMap);
			_localFinder = lpf;
		}
	}

	PathingSystem::~PathingSystem()
	{
		_pathRequestHandler->EndProcessing();
		ThreadManager::Instance()->RemoveThread("GlobalPathfinderThread");
		// xDelete(_pathRequestHandler); request handler will be deleted when thread ends its loop
		xDelete(_globalFinder);
		xDelete(_localFinder);
		xDelete(_pathingMap);
		for(unsigned int i = 0; i < _debugDrawers.size(); ++i)
		{
			xDeleteSafe(_debugDrawers[i]);
		}
	}

	void PathingSystem::Update(float ms)
	{
		ProcessFinishedPathRequests();
		_localFinder->FindUnitMoves(ms);
		static float updateTimer = 200.f;
		if( updateTimer <= 0.f )
		{
			_dbvh->Optimize();
			updateTimer = 200.f;
			//#ifdef _DEBUG
			if( _useDrawer[DbvhDrawer] )
			{
				_debugDrawers[DbvhDrawer]->ClearRender();
				_dbvh->DebugDraw(_debugDrawers[DbvhDrawer]);
				_debugDrawers[DbvhDrawer]->Draw();
			}
			//#endif
		}
		else
			updateTimer -= ms;

		//#ifdef _DEBUG
		if( InputManager::Instance()->CheckKeyWasPressed(OIS::KC_F5) )
		{
			_useDrawer[GridDrawer] = !_useDrawer[GridDrawer] ;
			if(_useDrawer[GridDrawer] )
			{
				_pathingMap->DrawGrid(_debugDrawers[GridDrawer]);
				_debugDrawers[GridDrawer]->Draw();
			}
			else
				_debugDrawers[GridDrawer]->ClearRender();
		}
		if( InputManager::Instance()->CheckKeyWasPressed(OIS::KC_F6) )
		{
			_useDrawer[GroundObstaclesDrawer] = !_useDrawer[GroundObstaclesDrawer];
			if(_useDrawer[GroundObstaclesDrawer])
			{
				_pathingMap->DrawObstacles(_debugDrawers[GroundObstaclesDrawer], CollisionGroups::Ground);
				_debugDrawers[GroundObstaclesDrawer]->Draw();
				_pathingMap->DrawObstacles(_debugDrawers[BuildingObstaclesDrawer], CollisionGroups::Building);
				_debugDrawers[BuildingObstaclesDrawer]->Draw();
			}
			else
			{
				_debugDrawers[GroundObstaclesDrawer]->ClearRender();
				_debugDrawers[BuildingObstaclesDrawer]->ClearRender();
			}
		}
		if( InputManager::Instance()->CheckKeyWasPressed(OIS::KC_F7) )
		{
			_useDrawer[DbvhDrawer] = !_useDrawer[DbvhDrawer];
			if(_useDrawer[DbvhDrawer])
			{
				_dbvh->DebugDraw(_debugDrawers[DbvhDrawer]);
				_debugDrawers[DbvhDrawer]->Draw();
			}
			else
				_debugDrawers[DbvhDrawer]->ClearRender();
		}
		//#endif
	}

	void PathingSystem::AddObstacle(PhysicalObject* object)
	{
		_ASSERT(object->FindComponent<CollisionComponent>() != 0);
		CollisionComponent* collision = object->FindComponent<CollisionComponent>();

		auto obstacle = collision->GetFootprint();
		if(obstacle != 0)
		{
			const Vector3& position = object->GetPositionAbsolute();
			int x,y;
			_pathingMap->FindTopLeftOfObstacleWithCenterPosition(obstacle, Vector2(position.x, position.z), &x, &y);
			_pathingMap->AddObstacle(obstacle, x, y);
			_globalFinder->UpdateObstacle(*obstacle, IntVector2(x,y));
		}
	}

	void PathingSystem::RemoveObstacle(PhysicalObject* object)
	{
		_ASSERT(object->FindComponent<CollisionComponent>() != 0);
		CollisionComponent* collision = object->FindComponent<CollisionComponent>();

		auto obstacle = collision->GetFootprint();
		if(obstacle != 0)
		{
			const Vector3& position = object->GetPositionAbsolute();
			int x,y;
			_pathingMap->FindTopLeftOfObstacleWithCenterPosition(obstacle, Vector2(position.x, position.z), &x, &y);
			_pathingMap->RemoveObstacle(obstacle, x, y);
			_globalFinder->UpdateObstacle(*obstacle, IntVector2(x,y));
		}
	}

	void PathingSystem::AddPathingObject(PhysicalObject* object)
	{
		_ASSERT(object->FindComponent<CollisionComponent>() != 0);
		CollisionComponent* collision = object->FindComponent<CollisionComponent>();

		PathingComponent* pathing = object->FindComponent<PathingComponent>();
		if(pathing != 0)
		{
			// Unit cannot move
			AddObstacle(object);
			return;
		}

		pathing->SetIsRequestedGlobalPath(false);
		pathing->SetIsRequestedLocalPath(false);
		pathing->SetIsFinishedMove(true);

		Vector3 size = object->GetSize();
		pathing->SetSizeInCells(IntVector2(
			std::max(1, (int)(size.x / _pathingMap->GetCellSize().x)), 
			std::max(1, (int)(size.z / _pathingMap->GetCellSize().y))));

		object->Moved() += &_onUnitMoved;

		// If object is to be added to pathing graph ( and so be able to request paths and influence other paths )
		// then add it to Dbvh and local finder
		pathing->SetFrameMove(Vector2(0.f,0.f));
		pathing->SetGlobalGoal(pathing->GetPosition());
		_localFinder->AddUnit(pathing);

		// Dbvh can be create only with >= 2 object
		if( _dbvh->IsCreated() == false && _initDbvhObject == 0 )
		{
			_initDbvhObject = object;
		}
		else if( _dbvh->IsCreated() == false )
		{
			_initDbvhObject = 0;
			CollisionComponent* initCollision = _initDbvhObject->FindComponent<CollisionComponent>();
			Array<std::pair<PhysicalObject*, BoundingBox>> objects;
			objects.push_back(std::make_pair(_initDbvhObject, initCollision->GetCollisionShape().GetBoundingBox()));
			objects.push_back(std::make_pair(object, collision->GetCollisionShape().GetBoundingBox()));
			_dbvh->CreateInitialBvh(objects);
		}
		else
		{
			_dbvh->AddObject(object, pathing->GetCollisionShape().GetBoundingBox());
		}
	}

	void PathingSystem::RemovePathingObject(PhysicalObject* object)
	{
		_ASSERT(object->FindComponent<CollisionComponent>() != 0);
		CollisionComponent* collision = object->FindComponent<CollisionComponent>();

		object->Moved() -= &_onUnitMoved;

		PathingComponent* pathing = object->FindComponent<PathingComponent>();
		if(pathing != 0)
		{
			_localFinder->RemoveUnit(pathing);
			_dbvh->RemoveObject(object);
		}
	}

	void PathingSystem::SetUnitGlobalGoal(Unit* unit, const Vector3& goal, 
		bool moveToGoal, bool reqestPath)
	{
		_ASSERT(unit->FindComponent<PathingComponent>());
		CollisionComponent* collision = unit->FindComponent<CollisionComponent>();
		PathingComponent* pathing = unit->FindComponent<PathingComponent>();

		pathing->SetGlobalGoal(Vector2(goal.x, goal.z));

		if(reqestPath)
			RequestPathToGoal(unit);

		if(moveToGoal)
		{
			pathing->SetIsRequestedLocalPath(true);
			pathing->SetLocalGoal(pathing->GetGlobalGoal());
		}
	}

	void PathingSystem::RequestPath(PhysicalObject* unit)
	{
		_ASSERT(unit->FindComponent<PathingComponent>() != 0);
		CollisionComponent* collision = unit->FindComponent<CollisionComponent>();
		PathingComponent* pathing = unit->FindComponent<PathingComponent>();

		pathing->SetIsFinishedMove(false);
		pathing->SetCurrentPathIndex(0);
		pathing->GetGlobalPath().clear();

		if(pathing->IsRequestedGlobalPath() == true)
		{
			pathing->SetIsRequestedGlobalPath(false);
			_pathRequestHandler->RemoveRequest(pathing);
		}

		// Request new global path
		_globalFinder->SetBlockFilter(pathing->GetCollsionFilter());
		IntVector2 begin, end;
		const Vector3& position = unit->GetPositionAbsolute();
		begin = _pathingMap->FindCellPositionFallsInto(Vector2(position.x, position.z));
		end = _pathingMap->FindCellPositionFallsInto(pathing->GetGlobalGoal());
		if( begin == end )
		{
			// SetSameCellGoal(); // TODO -> add only local pathfinding. It can be changed to something like 1 cell radius or 1 collision radius etc.
			// For now assume it finished moving
			return;
		}
		pathing->SetIsRequestedGlobalPath(true);
		_pathRequestHandler->AddRequest(begin, end, pathing);	
	}

	void PathingSystem::ProcessFinishedPathRequests()
	{
		_pathRequestHandler->Lock();
		PathRequest* req = _pathRequestHandler->GetFirstFinishedRequest();
		while(req != 0)
		{
			PathingComponent* pathing = req->Unit;
			pathing->SetIsRequestedGlobalPath(false);

			if(pathing->GetGlobalPath().size() > 1)
			{
				// GlobalPathfinder found some non-trivial path, so make unit follow it
				// TODO sometimes unit moves past first path cell before full path is calculated 
				// So need to check it and move to next cell, or change GPF, so that partial path 
				// can be accessed fast
				pathing->SetIsRequestedLocalPath(true);
				pathing->SetCurrentPathIndex(0);
				pathing->SetLocalGoal(pathing->GetGlobalPath()[pathing->GetCurrentPathIndex() + 1]);
			}
			else if(pathing->GetGlobalPath().size() == 1) 
			{
				// Goal is on straight line - only go to final goal
				pathing->SetIsRequestedLocalPath(true);
				pathing->SetLocalGoal(pathing->GetGlobalGoal());
			}
			else // There's no path
			{
				pathing->SetIsRequestedLocalPath(false);
				pathing->SetGlobalGoal(pathing->GetPosition());
				pathing->SetLocalGoal(pathing->GetPosition());
				pathing->SetIsFinishedMove(true);
			}

			_pathRequestHandler->RemoveFinishedRequest();
			req = _pathRequestHandler->GetFirstFinishedRequest();
		}
		_pathRequestHandler->Unlock();
	}

	Vector2 PathingSystem::GetUnitNextMove(Unit* unit)
	{
		_ASSERT(unit->FindComponent<PathingComponent>() != 0);
		PathingComponent* pathing = unit->FindComponent<PathingComponent>();

		return pathing->GetFrameMove();
	}

	bool PathingSystem::ShouldUnitMove(Unit* unit)
	{
		_ASSERT(unit->FindComponent<PathingComponent>() != 0);
		PathingComponent* pathing = unit->FindComponent<PathingComponent>();

		return pathing->GetFrameMove().squaredLength() > 0.1f;
	}

	void PathingSystem::SetUnitHoldPosition(Unit* unit, bool holdPos)
	{
		_ASSERT(unit->FindComponent<PathingComponent>() != 0);
		PathingComponent* pathing = unit->FindComponent<PathingComponent>();
		pathing->SetIsAllowDisplacement(!holdPos);
	}

	void PathingSystem::AbandonPath(PhysicalObject* unit)
	{
		_ASSERT(unit->FindComponent<PathingComponent>() != 0);
		PathingComponent* pathing = unit->FindComponent<PathingComponent>();

		pathing->GetGlobalPath().clear();
		if(pathing->IsRequestedGlobalPath() == true)
		{
			_pathRequestHandler->RemoveRequest(pathing);
		}

		pathing->SetIsFinishedMove(true);
		//pathUnit->Unit.SetAllowDisplacement(false);
		pathing->SetIsRequestedGlobalPath(false);
		pathing->SetIsRequestedLocalPath(false);
		pathing->SetGlobalGoal(pathing->GetPosition());
		pathing->SetLocalGoal(pathing->GetPosition());
	}

	void PathingSystem::OnUnitMoved(SceneObject* unit)
	{
		_ASSERT(unit->FindComponent<PathingComponent>() != 0);
		PathingComponent* pathing = unit->FindComponent<PathingComponent>();

		_localFinder->MoveUnit( pathing );
		_dbvh->ObjectChanged(unit, pathing->GetCollisionShape().GetBoundingBox());

		_pathRequestHandler->Lock();
		if(pathing->IsRequestedLocalPath()) // If unit have some path to follow
		{
			// Check if unit reach goal
			// if it is sub-goal check within tolerance
			if(pathing->GetCurrentPathIndex() >= (int)pathing->GetGlobalPath().size() - 2 ||
				pathing->GetGlobalPath().size() == 0) // Reached final goal cell
			{
				if(pathing->GetGlobalGoal().squaredDistance(pathing->GetPosition()) < 0.1f) // Reach final goal position
				{
					pathing->SetIsFinishedMove(true);
					//pathUnit->Unit.SetAllowDisplacement(false);
					pathing->SetIsRequestedLocalPath(false);
				}
			}
			else
			{
				// TODO make '100.f' ( distance where target is considered to be reached ) configurable parameter
				if( pathing->GetLocalGoal().squaredDistance(pathing->GetPosition()) < 100.f ) // Reached next goal cell 
				{
					pathing->SetCurrentPathIndex(pathing->GetCurrentPathIndex() + 1);
					if(pathing->GetCurrentPathIndex() >= (int)pathing->GetGlobalPath().size() - 2) // Reached final cell
					{
						pathing->SetLocalGoal(pathing->GetGlobalGoal());
					}
					else
					{
						pathing->SetLocalGoal(
							pathing->GetGlobalPath()[pathing->GetCurrentPathIndex() + 1]);
					}
				}
			}
		}

		_pathRequestHandler->Unlock();
	}

	void PathingSystem::OnUnitRotated(SceneObject* unit)
	{
		//PathUnit* pathUnit = GetPathUnit( static_cast<Unit*>(unit)->GetPathingHandle() );

		// _localFinder->RotateUnit( pathUnit, 0 );
	}

	void PathingSystem::SetRootDebugNode(Ogre::SceneNode* debugnode)
	{
		_debugNode = debugnode;
		for(unsigned int i = 0; i < _debugDrawers.size(); ++i)
		{
			_debugDrawers[i]->AttachToScene(debugnode);
		}
	}

	IPathingQuery* PathingSystem::CreatePathingQuery()
	{
		return new BoxDbvhQuery(_dbvh);
	}

	float PathingSystem::GetDistanceBetweenObjects(PhysicalObject* obj1, PhysicalObject* obj2)
	{
		_ASSERT(obj1->FindComponent<CollisionComponent>() != 0);
		_ASSERT(obj2->FindComponent<CollisionComponent>() != 0);

		CollisionComponent* col1 = obj1->FindComponent<CollisionComponent>();
		return col1->GetDistanceToOtherObject(obj2);
	}

	struct Check3dCollision : public Dyscriminator
	{
		PhysicalObject* TestObject;
		bool operator()(PhysicalObject* object)
		{
			// For now only check bounding box
			if( object == TestObject )
				return false;

			auto aabb = object->GetBoudningBox();
			auto testaabb = TestObject->GetBoudningBox();
			return aabb.intersects(testaabb);
		}
	};

	Array<PhysicalObject*>& PathingSystem::Find3dCollisions(PhysicalObject* collider)
	{
		_lastCollisions.clear();
		CollisionComponent* collision = collider->FindComponent<CollisionComponent>();
		if(collision == 0)
			return _lastCollisions;

		// Use query to accept 2d collision only if object collide in 3d
		BoxDbvhQuery query(_dbvh);
		Check3dCollision checkCollision;
		checkCollision.TestObject = collider;
		query.SetTestShape(&collision->GetCollisionShape());
		query.Execute(checkCollision);

		_lastCollisions.resize(query.GetObjectsHit().size());
		for(unsigned int i = 0; i < _lastCollisions.size(); ++i)
			_lastCollisions[i] = query.GetObjectsHit()[i];

		return _lastCollisions;
	}
}