#include "ConstructionSiteFactory.h"
#include "Unit.h"
#include "ConstructionSiteData.h"
#include "GameWorld.h"
#include "IGameObjectPool.h"
#include "ISceneObjectSpawner.h"
#include "Map.h"
#include <PathFindingUniformGridPathingMap.h>
#include <StringUtils.h>

#include <OgreSceneManager.h>
#include <OgreManualObject.h>
#include <OgreMesh.h>
#include <OgreMeshManager.h>
#include <OgreSubMesh.h>
#include <OgreVertexIndexData.h>

namespace FlagRTS
{
	ConstructionSiteFactory::ConstructionSiteFactory(Ogre::SceneManager* ogreMgr, 
		PathFinding::UniformGridPathingMap* obstacleMap) :
		_ogreMgr(ogreMgr),
		_obstacleMap(obstacleMap)
	{

	}

	Ogre::MeshPtr GenerateCSiteMesh(Unit* building, const string& name);

	Unit* ConstructionSiteFactory::CreateSite(Unit* building, int owner)
	{
		// Create manual mesh based on building footprint
		string name = building->GetName() + "CSiteMesh";
		Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().getByName(name, "Base");
		if( mesh.isNull() )
		{
			GenerateCSiteMesh(building, name.c_str());
		}

		UnitDefinition* csiteDef = xNew0(UnitDefinition);

		csiteDef->SetModelName(name);
		csiteDef->SetInheritsScale(false);
		csiteDef->SetIsIndependent(true);
		csiteDef->SetCountsAsPlayerUnit(false);
		csiteDef->SetScale(Vector3(1.f,1.f,1.f));
		/*
		auto khsupp = xNew0(ConstructionSiteTypeDataSupplier);
		csiteDef->SetKindDataHandleSupplier(khsupp);
		csiteDef->SetKindSpecificDataHandle(khsupp->GetKindSpecificDataHandle());
		csiteDef->SetObjectDataHandleSupplier(
			xNew0(ConstructionSiteObjectDataSupplier));
			*/
		csiteDef->SetAnimations(
			xNew0(AnimationDefinitionMap));
		csiteDef->SetPathingBlockedGroups(building->GetPathingBlockedGroups());
		csiteDef->SetPathingGroup(building->GetPathingGroup());
		csiteDef->GetObjectClasses().push_back(CopyChar("ConstructionSite"));

		Unit* csite = static_cast<Unit*>(GameInterfaces::GetGameObjectPool()->
			Create(csiteDef, owner));

		// csite->GetObjectSpecificData<ConstructionSiteObjectData>()->Building = building;

		return csite;
	}

	void ConstructionSiteFactory::DestroySite(Unit* site)
	{
		auto def = site->GetUnitDefinition();
		GameInterfaces::GetSceneObjectSpawner()->DestroySceneObject(site);
		xDelete(def);
	}

	Unit* ConstructionSiteFactory::GetConstructedBuilding(Unit* site)
	{
		ConstructionSiteObjectData* data = site->GetObjectSpecificData<ConstructionSiteObjectData>();
		return data->Building;
	}

	Ogre::Mesh* ConstructionSiteFactory::GenerateCSiteMesh(Unit* building, const char* name)
	{
		auto footprint = building->GetFootprint();
		auto bbox = building->GetEntity()->getBoundingBox();
		Vector3 center = building->GetPhysicalObjectDefinition()->GetSize() * 0.5f;
		float height = 1.f;
		Vector2 cellSize = _obstacleMap->GetCellSize();

		auto manMesh = _ogreMgr->createManualObject();
		manMesh->setCastShadows(true);
		manMesh->setDynamic(true);
		manMesh->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAIN);

		uint16 cellIdx = 0;
		manMesh->estimateVertexCount(footprint->GetRowCount()*footprint->GetColumnCount() * 4);
		manMesh->estimateIndexCount(footprint->GetRowCount()*footprint->GetColumnCount() * 6);
		manMesh->begin("CSiteMat");
		for (int y = 0; y < footprint->GetRowCount(); ++y)
		{ 
			for (int x = 0; x < footprint->GetColumnCount(); ++x)
			{ 
				if(footprint->GetAt(y,x) != 0)
				{
					Vector2 corner(x * cellSize.x - center.x, y * cellSize.y - center.z);

					manMesh->position(
						Vector3(corner.x,height,corner.y));

					manMesh->position(
						Vector3(corner.x+cellSize.x,height,corner.y));

					manMesh->position(
						Vector3(corner.x,height,corner.y+cellSize.y));

					manMesh->position(
						Vector3(corner.x+cellSize.x,height,corner.y+cellSize.y));

					manMesh->triangle(cellIdx, cellIdx+3, cellIdx+1);
					manMesh->triangle(cellIdx+3, cellIdx, cellIdx+2);
					cellIdx += 4;
				}
			}
		}
		manMesh->end();
		Ogre::MeshPtr msh = manMesh->convertToMesh(name, "Base");
		msh->_setBounds(Ogre::AxisAlignedBox(-center.x, 0.f, -center.z, center.x, 2*center.y, center.z));
		msh->_setBoundingSphereRadius(building->GetEntity()->getBoundingRadius());
		_ogreMgr->destroyManualObject(manMesh);

		return msh.getPointer();
	}
}