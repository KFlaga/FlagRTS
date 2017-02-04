#include "PathingSystem.h"
#include <PathFindingUniformGridPathingMap.h>
#include "PathingDrawer.h"

#include <InputManager.h>
#include <KeyBindings.h>

#include <PathFindingUniformGridPathingMap.h>
#include <PathFindingUniformGridJumpPointFinder.h>
#include <PathFindingUniformGridVariedSizeJumpFinder.h>
#include <PathFindingIPathUnit.h>
#include <PathFindingForcesPathFinder.h>
#include <PathFindingUnitIgnorePathFinder.h>
#include <PathFindingDbvh.h>
#include <PathFindingDbvhUnitUnitPairer.h>
#include <PathFindingForcesPathFinder.h>
#include "PathingPathUnit.h"
#include "PathingQuery.h"

#include <OgreResourceGroupManager.h>
#include "Unit.h"
#include "GameWorld.h"
#include <Thread.h>
#include <ThreadManager.h>
#include "PathRequestHandler.h"

#include "CollisionGroup.h"
#include "TerrainBase.h"
#include <PathFindingHeightMapConverter.h>

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
	class JumpPointFinder : public PathFinding::IGlobalPathFinder
	{
	public:
		enum FinderType
		{
			NormalJP,
			VariedSizeJP
		};

	private:
		std::map<int, IGlobalPathFinder*> _perFilterPathfinders;
		PathFinding::UniformGridPathingMap* _pathingMap;
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

		void UpdateObstacle(const PFArray2d<CollisionFilter>& obstacle, const IntVector2& topLeftCell)
		{
			for(auto pfIt = _perFilterPathfinders.begin(); pfIt != _perFilterPathfinders.end(); ++pfIt)
				pfIt->second->UpdateObstacle(obstacle, topLeftCell);
		}
	};

	inline PathUnit* GetPathUnit(const size_t handle)
	{
		return reinterpret_cast<PathUnit*>(handle);
	}

	inline size_t GetUnitHandle(const PathUnit* unit)
	{
		return reinterpret_cast<size_t>(unit);
	}

	struct PathUnitGetter
	{
		inline IPathUnit<PFVector2>* operator()(PhysicalObject* obj) const
		{
			return GetPathUnit(obj->GetPathingHandle())->Unit;
		}
	};

	class Dbvh : public PathFinding::Dbvh<PathFinding::Box>
	{
	public:
		Dbvh(float minMergeBen, float minRotBen) :
			PathFinding::Dbvh<PathFinding::Box>(minMergeBen, minRotBen)
		{ }
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
		_pathRequestHandler(0)
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

		_dbvh = xNew2(Dbvh, 0.9f, 0.75f);
		InitGlobalPathFinder(rootNode);
		InitLocalPathFinder(rootNode);

		_goalTolerance = XmlUtility::XmlGetFloat(rootNode->first_node("PathGoalTolerance"), "value");
		_isToleranceRelative = XmlUtility::XmlGetBool(rootNode->first_node("PathGoalToleranceRelative"), "value");
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
			x, y, PFVector2(cellSize, cellSize), filter);
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
			auto uuPairer = new DbvhUnitUnitLookupPairer2d<Box,
				FindUnitsWithinLookupShapeQuery2d<Box,
				PhysicalObject, PathUnitGetter>>(_dbvh);
			ForcesPathFinder* lpf = xNew2(ForcesPathFinder, _pathingMap, uuPairer);
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
			UnitIgnorePathFinder* lpf = xNew1(UnitIgnorePathFinder, _pathingMap);
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
		auto obstacle = object->GetFootprint();
		const Vector3& position = object->GetPositionAbsolute();
		int x,y;
		_pathingMap->FindTopLeftOfObstacleWithCenterPosition(obstacle, Vector2(position.x, position.z), &x, &y);
		_pathingMap->AddObstacle(obstacle, x, y);
		_globalFinder->UpdateObstacle(*obstacle, IntVector2(x,y));
	}

	void PathingSystem::RemoveObstacle(PhysicalObject* object)
	{
		auto obstacle = object->GetFootprint();
		const Vector3& position = object->GetPositionAbsolute();
		int x,y;
		_pathingMap->FindTopLeftOfObstacleWithCenterPosition(obstacle, Vector2(position.x, position.z), &x, &y);
		_pathingMap->RemoveObstacle(obstacle, x, y);
		_globalFinder->UpdateObstacle(*obstacle, IntVector2(x,y));
	}

	void PathingSystem::AddPathingObject(PhysicalObject* object)
	{
		// Create PathUnit for object and set some values
		PathUnit* pathUnit = CreateShapedPathUnit(
			(CollisionShapeType)object->GetPhysicalObjectDefinition()->GetPathingShape(),
			CollisionFilter(object->GetPathingGroup(), object->GetPathingBlockedGroups()));

		pathUnit->Owner = object;

		if(object->GetFinalType() == GetTypeId<Unit>())
		{
			// Only units can move
			pathUnit->Unit->SetMaxSpeed(static_cast<Unit*>(object)->GetSpeed() );
		}
		pathUnit->Unit->SetIsPathRequested(false);

		const Vector3& position = object->GetPositionAbsolute();
		pathUnit->Unit->SetPosition(Vector2(position.x, position.z));
		SetPathUnitShape(pathUnit, object);

		Vector3 size = object->GetSize();
		pathUnit->CellSize = IntVector2(
			std::max(1, (int)(size.x / _pathingMap->GetCellSize().x)), 
			std::max(1, (int)(size.z / _pathingMap->GetCellSize().y)));

		object->SetPathingHandle(GetUnitHandle(pathUnit));
		object->Moved() += &_onUnitMoved;


		if( object->GetAddToPathingGraph() )
		{
			// If object is to be added to pathing graph ( and so be able to request paths and influence other paths )
			// then add it to Dbvh and local finder
			pathUnit->Unit->SetFrameMove(Vector2(0.f,0.f));
			pathUnit->Unit->SetGoal(pathUnit->Unit->GetPosition());
			_localFinder->AddUnit(pathUnit->Unit);

			_pathUnits.insert(pathUnit, object);
			// Dbvh can be create only with >= 2 object
			if( _pathUnits.size() == 2 )
			{
				PFArray<std::pair<PhysicalObject*, PathFinding::Box>> objects;
				auto it = _pathUnits.begin();
				objects.push_back(std::make_pair(it->Value, GetPathUnitBoundingBox(it->Key)));
				++it;
				objects.push_back(std::make_pair(it->Value, GetPathUnitBoundingBox(it->Key)));
				_dbvh->CreateInitialBvh(objects);
			}
			else if( _pathUnits.size() > 2 )
			{
				_dbvh->AddObject(object, GetPathUnitBoundingBox(pathUnit));
			}
		}
	}

	void PathingSystem::RemovePathingObject(PhysicalObject* object)
	{
		if(object->GetPathingHandle() == 0)
			return;

		PathUnit* pathUnit = GetPathUnit(object->GetPathingHandle());
		object->Moved() -= &_onUnitMoved;
		object->SetPathingHandle(0);

		if( object->GetAddToPathingGraph() )
		{
			_localFinder->RemoveUnit(pathUnit->Unit);
			_dbvh->RemoveObject(object);
		}

		xDelete(pathUnit);
	}

	void PathingSystem::SetUnitGlobalGoal(Unit* unit, const Vector3& goal, 
		bool moveToGoal, bool reqestPath)
	{
		if(unit->GetPathingHandle() == 0)
			return;

		PathUnit* pathUnit = GetPathUnit(unit->GetPathingHandle());
		pathUnit->FinalGoal = Vector2(goal.x, goal.z);

		if(reqestPath)
			RequestPathToGoal(unit);

		if(moveToGoal)
		{
			pathUnit->Unit->SetIsPathRequested(true);
			pathUnit->Unit->SetGoal(pathUnit->FinalGoal);
		}
	}

	void SetSameCellGoal() { }

	void PathingSystem::RequestPathToGoal(Unit* unit)
	{
		if(unit->GetPathingHandle() == 0)
			return;

		PathUnit* pathUnit = GetPathUnit(unit->GetPathingHandle());
		pathUnit->FinishedMove = false;
		pathUnit->NextPathIdx = 1;
		pathUnit->Path.clear();

		if(pathUnit->HavePathRequestPending == true)
		{
			pathUnit->HavePathRequestPending = false;
			_pathRequestHandler->RemoveRequest(pathUnit);
		}

		// Request new global path
		_globalFinder->SetBlockFilter(pathUnit->Unit->GetCollsionFilter());
		IntVector2 begin, end;
		const Vector3& position = unit->GetPositionAbsolute();
		pathUnit->Unit->SetGoal(Vector2(position.x, position.z)); // Comment line, so unit will move towards goal awaiting for path, rather than staying in place
		begin = _pathingMap->FindCellPositionFallsInto(Vector2(position.x, position.z));
		end = _pathingMap->FindCellPositionFallsInto(pathUnit->FinalGoal);
		if( begin == end )
		{
			SetSameCellGoal(); // TODO -> add only local pathfinding. It can be changed to something like 1 cell radius or 1 collision radius etc.
			// For now assume it finished moving
			return;
		}
		pathUnit->HavePathRequestPending = true;
		_pathRequestHandler->AddRequest(begin, end, pathUnit);	
	}

	void PathingSystem::ProcessFinishedPathRequests()
	{
		_pathRequestHandler->Lock();
		PathRequest* req = _pathRequestHandler->GetFirstFinishedRequest();
		while(req != 0)
		{
			PathUnit* pathUnit = req->Unit;
			pathUnit->HavePathRequestPending = false;

			if(pathUnit->Path.size() > 1)
			{
				// GlobalPathfinder found some non-trivial path, so make unit follow it
				// TODO sometimes unit moves past first path cell before full path is calculated 
				// So need to check it and move to next cell, or change GPF, so that partial path 
				// can be accessed fast
				pathUnit->Unit->SetIsPathRequested(true);
				pathUnit->NextPathIdx = 1;
				pathUnit->Unit->SetGoal(pathUnit->Path[pathUnit->NextPathIdx]);
			}
			else if(pathUnit->Path.size() == 1) 
			{
				// Goal is on straight line - only go to final goal
				pathUnit->Unit->SetIsPathRequested(true);
				pathUnit->Unit->SetGoal(pathUnit->FinalGoal);
			}
			else // There's no path
			{
				pathUnit->Unit->SetIsPathRequested(false);
				pathUnit->Unit->SetGoal(pathUnit->Unit->GetPosition());
				pathUnit->FinishedMove = true;
			}

			_pathRequestHandler->RemoveFinishedRequest();
			req = _pathRequestHandler->GetFirstFinishedRequest();
		}
		_pathRequestHandler->Unlock();
	}

	Vector2 PathingSystem::GetUnitNextMove(Unit* unit)
	{
		_ASSERT( unit->GetPathingHandle() != 0 );

		PathUnit* pathUnit = GetPathUnit(unit->GetPathingHandle());
		return pathUnit->Unit->GetFrameMove();
	}

	bool PathingSystem::ShouldUnitMove(Unit* unit)
	{
		if(unit->GetPathingHandle() == 0)
			return false;

		PathUnit* pathUnit = GetPathUnit(unit->GetPathingHandle());
		return pathUnit->Unit->GetFrameMove().squaredLength() > 0.1f;
	}

	void PathingSystem::SetUnitHoldPosition(Unit* unit, bool holdPos)
	{
		if( unit->GetPathingHandle() != 0 )
		{
			PathUnit* pathUnit = GetPathUnit(unit->GetPathingHandle());
			//pathUnit->Unit.SetAllowDisplacement(!holdPos);
		}
	}

	inline void ResetGoal(PathUnit* pathUnit)
	{
		_ASSERT( pathUnit != 0 );

		pathUnit->FinishedMove = true;
		//pathUnit->Unit.SetAllowDisplacement(false);
		pathUnit->Unit->SetIsPathRequested(false);
		pathUnit->Unit->SetGoal(pathUnit->Unit->GetPosition());
	}

	void PathingSystem::AbandonPath(Unit* unit)
	{
		_ASSERT( unit->GetPathingHandle() != 0 );

		PathUnit* pathUnit = GetPathUnit(unit->GetPathingHandle());
		pathUnit->Path.clear();
		if(pathUnit->HavePathRequestPending == true)
		{
			_pathRequestHandler->RemoveRequest(pathUnit);
		}
		ResetGoal(pathUnit);
	}

	void PathingSystem::OnUnitMoved(SceneObject* unit)
	{
		PathUnit* pathUnit = GetPathUnit( static_cast<Unit*>(unit)->GetPathingHandle() );
		const Vector3& pos = unit->GetPositionAbsolute();
		pathUnit->Unit->SetPosition(Vector2(pos.x, pos.z));
		UpdatePathUnitShape(pathUnit, static_cast<PhysicalObject*>(unit));

		// Dont update paths for unit not in graph
		if( static_cast<PhysicalObject*>(unit)->GetAddToPathingGraph() == false )
			return;

		_localFinder->MoveUnit( pathUnit->Unit );
		_dbvh->ObjectChanged(unit, GetPathUnitBoundingCircle(pathUnit));

		_pathRequestHandler->Lock();
		if(pathUnit->Unit->GetIsPathRequested()) // If unit have some path to follow
		{
			// Check if unit reach goal
			// if it is sub-goal check within tolerance
			if(pathUnit->NextPathIdx >= (int)pathUnit->Path.size() - 1 ||
				pathUnit->Path.size() == 0) // Reached final goal cell
			{
				if(pathUnit->FinalGoal.squaredDistance(pathUnit->Unit->GetPosition()) < 0.1f) // Reach final goal position
				{
					ResetGoal(pathUnit);
				}
			}
			else
			{
				// TODO make '100.f' ( distance where target is considered to be reached ) configurable parameter
				if( pathUnit->Unit->GetGoal().squaredDistance(
					pathUnit->Unit->GetPosition()) < 100.f ) // Reached next goal cell 
				{
					pathUnit->NextPathIdx += 1;
					if(pathUnit->NextPathIdx >= (int)pathUnit->Path.size() - 1) // Reached final cell
					{
						pathUnit->Unit->SetGoal(pathUnit->FinalGoal);
					}
					else
					{
						pathUnit->Unit->SetGoal(
							pathUnit->Path[pathUnit->NextPathIdx]);
					}
				}
			}
		}
		else
			pathUnit->Unit->SetGoal(pathUnit->Unit->GetPosition());
		_pathRequestHandler->Unlock();
	}

	void PathingSystem::OnUnitRotated(SceneObject* unit)
	{
		PathUnit* pathUnit = GetPathUnit( static_cast<Unit*>(unit)->GetPathingHandle() );

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

	IBoxPathingQuery* PathingSystem::CreateBoxPathingQuery()
	{
		return new BoxDbvhBoxObjectPathingQuery(_dbvh);
	}

	void PathingSystem::DestroyBoxPathingQuery(IBoxPathingQuery* query)
	{
		delete query;
	}

	float PathingSystem::GetDistanceBetweenObjects(PhysicalObject* obj1, PhysicalObject* obj2)
	{
		_ASSERT( obj1->GetPathingHandle() != 0 );
		_ASSERT( obj2->GetPathingHandle() != 0 );
		return GetPathUnit(obj1->GetPathingHandle())->Unit->GetDitanceTo(
			GetPathUnit(obj2->GetPathingHandle())->Unit);
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
		if(collider->GetPathingHandle() == 0)
			return _lastCollisions;

		PathUnit* collideUnit = GetPathUnit(collider->GetPathingHandle());
		// Use query to accept 2d collision only if object collide in 3d
		PathFinding::DbvhQuery<Box, Box, PhysicalObject> query;
		Check3dCollision checkCollision;
		checkCollision.TestObject = collider;
		query.SetTestShape(GetPathUnitBoundingBox(collideUnit));
		query.Execute(_dbvh, checkCollision);

		_lastCollisions.resize(query.GetObjectsHit().size());
		for(unsigned int i = 0; i < _lastCollisions.size(); ++i)
			_lastCollisions[i] = query.GetObjectsHit()[i];

		return _lastCollisions;
	}
}