#include "ConstructionPlanner.h"
#include "ObjectOutline.h"
#include "UnitDefinition.h"
#include <PathFindingUniformGridPathingMap.h>
#include "GameWorld.h"
#include "ISceneObjectSpawner.h"
#include "IGameObjectPool.h"
#include "Map.h"
#include "PathingSystem.h"

#include <Ogre\OgreSceneManager.h>
#include "CellColorer.h"

namespace FlagRTS
{	
	struct HitNotObstacles : public Dyscriminator
	{
		bool operator()(PhysicalObject* object)
		{
			return !object->HaveFootprint();
		}
	};

	ConstructionPlanner::ConstructionPlanner(Ogre::SceneManager* ogreMgr, 
		PathFinding::UniformGridPathingMap* obstacleMap) :
	_obstacleMap(obstacleMap),
		_currentOutline(0),
		_ogreMgr(ogreMgr),
		_topLeftCell(-1,-1)
	{
		_validColorer = xNew2(CellColorer,ogreMgr, "OutlineCellGreen");
		_validColorer->AttachToScene(
			ogreMgr->getRootSceneNode());
		_validColorer->SetCellSize(obstacleMap->GetCellSize());

		_invalidColorer = xNew2(CellColorer,ogreMgr, "OutlineCellRed");
		_invalidColorer->AttachToScene(
			ogreMgr->getRootSceneNode());
		_invalidColorer->SetCellSize(obstacleMap->GetCellSize());
	}

	ConstructionPlanner::~ConstructionPlanner()
	{
		if(_currentOutline != 0)
		{
			GameInterfaces::GetSceneObjectSpawner()->DespawnSceneObject(_currentOutline);
			_currentOutline->UnloadResources(_ogreMgr);
			xDelete(_currentOutline);
		}

		_validColorer->DetachFromScene();
		xDelete(_validColorer);
		_invalidColorer->DetachFromScene();
		xDelete(_invalidColorer);
	}

	void ConstructionPlanner::Update(float ms)
	{
		if(_currentOutline != 0)
		{
			_validColorer->Draw();
			_invalidColorer->Draw();
		}
	}

	void ConstructionPlanner::SetPlannedObjectPosition(const Vector3& terrainPosition)
	{
		_ASSERT( _result.UnitDef != 0);
		_ASSERT( _currentOutline != 0);
		auto footprint = _result.UnitDef->GetFootprint();
		// Find new center-cell and check if its changed
		IntVector2 newTopLeft = _obstacleMap->FindTopLeftOfObstacleWithCenterPosition(
			footprint, Vector2(terrainPosition.x, terrainPosition.z));

		if( _topLeftCell != newTopLeft )
		{
			// If changed, find new terrain center
			_topLeftCell = newTopLeft;

			Vector3 newCenter;
			newCenter.x = ((float)newTopLeft.X + (float)footprint->GetColumnCount() * 0.5f) * _obstacleMap->GetCellSize().x;
			newCenter.z = ((float)newTopLeft.Y + (float)footprint->GetRowCount() * 0.5f) * _obstacleMap->GetCellSize().y;
			newCenter.y = GameWorld::GlobalWorld->GetMap()->GetTerrainHeight(
				newCenter.x, newCenter.z);

			// Move outline to center
			_currentOutline->SetPosition(newCenter);
			_result.PlannedPosition = newCenter;

			bool canBePlaced = true;
			_validColorer->ClearRender();
			_validColorer->ClearCells();
			_invalidColorer->ClearRender();
			_invalidColorer->ClearCells();
			// Check for obstacles on footprint + add cells to colorer
			for( int dy = 0; dy < footprint->GetRowCount(); ++dy)
			{
				for( int dx = 0; dx < footprint->GetColumnCount(); ++dx)
				{
					if( footprint->GetAt( dy, dx ).Value != 0 )
					{
						if( newTopLeft.X + dx < 0 ||
							newTopLeft.X + dx >= _obstacleMap->GetCellCountX() ||
							newTopLeft.Y + dy < 0 ||
							newTopLeft.Y + dy >= _obstacleMap->GetCellCountY())
						{
							canBePlaced = false;
						}
						else if(_obstacleMap->GetCell( newTopLeft.Y + dy, newTopLeft.X + dx ).
								BlocksUnit(footprint->GetAt(dy, dx)))
						{
							canBePlaced = false;
							_invalidColorer->AddCell( newTopLeft.X + dx, newTopLeft.Y + dy );
						}
						else
						{
							_validColorer->AddCell( newTopLeft.X + dx, newTopLeft.Y + dy );
						}
					}
				}
			}

			if( canBePlaced )
			{
				auto query = GameWorld::GlobalWorld->GetPathingSystem()->CreateBoxPathingQuery();
				auto& pos = _currentOutline->GetPositionAbsolute();
				auto& hsize = _currentOutline->GetHalfSize();
				query->SetTestShape( BoundingBox(PFVector2(pos.x - hsize.x, pos.z - hsize.z), 
					PFVector2(pos.x + hsize.x, pos.z + hsize.z)));
				HitNotObstacles dysc;
				query->Execute(dysc);

				_result.IsLocationValid = query->GetObjectsHit().size() == 0;
				GameWorld::GlobalWorld->GetPathingSystem()->DestroyBoxPathingQuery(query);
			}
			else
				_result.IsLocationValid = false;

		}

		_currentOutline->SetIsStateValid(_result.IsLocationValid);
	}

	bool ConstructionPlanner::CheckPlanIsValid(UnitDefinition* plannedBuildingDef, 
		const Vector3& terrainPosition, uint8 player)
	{
		auto footprint = plannedBuildingDef->GetFootprint();
		IntVector2 topLeft = _obstacleMap->FindTopLeftOfObstacleWithCenterPosition(
			footprint, Vector2(terrainPosition.x, terrainPosition.z));
		for( int dy = 0; dy < footprint->GetRowCount(); ++dy)
		{
			for( int dx = 0; dx < footprint->GetColumnCount(); ++dx)
			{
				if( footprint->GetAt( dy, dx ).Value != 0 )
				{
					if( (_obstacleMap->GetCell( topLeft.Y + dy, topLeft.X + dx ).Blocking &
						footprint->GetAt(dy, dx).Groups) != 0 )
					{
						return false;
					}
				}
			}
		}
		return true;
	}

	const PlanningResult& ConstructionPlanner::AcceptPlan()
	{
		AbandonPlan();
		return _result;
	}

	void ConstructionPlanner::AbandonPlan()
	{
		_ASSERT( _currentOutline != 0 );
		_validColorer->ClearCells();
		_validColorer->ClearRender();
		_invalidColorer->ClearCells();
		_invalidColorer->ClearRender();

		GameInterfaces::GetSceneObjectSpawner()->DespawnSceneObject(_currentOutline);
		_currentOutline->UnloadResources(_ogreMgr);
		xDelete(_currentOutline);
		_currentOutline = 0;
	}

	void ConstructionPlanner::StartNewPlan(UnitDefinition* plannedBuildingDef, 
		const Vector3& startPosition)
	{
		_result.UnitDef = plannedBuildingDef;
		if( _currentOutline != 0)
		{
			// Should not hit here ( old plan was not abadonded )
			GameInterfaces::GetSceneObjectSpawner()->DespawnSceneObject(_currentOutline);
			_currentOutline->UnloadResources(_ogreMgr);
			xDelete(_currentOutline);
			_currentOutline = 0;
		}

		_currentOutline = xNew1(ObjectOutline, plannedBuildingDef);
		_currentOutline->LoadResources(_ogreMgr);
		GameInterfaces::GetSceneObjectSpawner()->SpawnSceneObject(_currentOutline,
			SpawnInfo( Quaternion::IDENTITY, startPosition, false));

		_topLeftCell = IntVector2(-1,-1);
		SetPlannedObjectPosition(startPosition);
	}
}