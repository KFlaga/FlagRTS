#include "TerrainProjectionMarker.h"
#include <OgreFrustum.h>
#include <OgreMaterial.h>
#include <OgrePass.h>
#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>

#include "GameWorld.h"
#include "Map.h"
#include <Terrain\OgreTerrain.h>
#include "Player.h"

namespace FlagRTS
{
	// TODO modify Ogre TerrainMaterialGenerator, so that this would work


	TerrainProjectionMarker::TerrainProjectionMarker(TerrainProjectionMarkerDefinition* tpmDef) :
		SceneMarker(tpmDef),
		_updateProjection(this)
	{
		SetFinalType(GetTypeId<TerrainProjectionMarker>());
		_onMoved += &_updateProjection;
		_onRotated += &_updateProjection;
	}

	TerrainProjectionMarker::~TerrainProjectionMarker()
	{

	}

	void TerrainProjectionMarker::AddMaterial(const string& matName)
	{
		// check if material is already added or there's no material
		if( _targetMaterials.find(matName) != _targetMaterials.end() ||
			matName.empty() )
		{
			return;
		}

		string matName2 = "StoreMat";

		// get the material ptr
		Ogre::MaterialPtr mat = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName(matName);

		// create a new pass in the material to render the decal
		Ogre::Pass* pass = mat->getTechnique(0)->createPass();

		// set up the decal's texture unit
		Ogre::TextureUnitState *texState = pass->createTextureUnitState(GetTextureName());
		texState->setProjectiveTexturing(true, _projectionFrustum);
		texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
		texState->setTextureFiltering(Ogre::FO_POINT, Ogre::FO_LINEAR, Ogre::FO_NONE);
		texState->setAlphaOperation(Ogre::LBX_ADD);

		// set our pass to blend the decal over the model's regular texture
		pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		pass->setDepthBias(2.5f, 2.5f);
		pass->setDepthCheckEnabled(false);

		// set the decal to be self illuminated instead of lit by scene lighting
		pass->setLightingEnabled(false);

		// save pass in map
		_targetMaterials[matName] = pass;
	}

	void TerrainProjectionMarker::RemoveMaterial(const string& matName)
	{
		// remove our pass from the given material
		_targetMaterials[matName]->getParent()->removePass(
			_targetMaterials[matName]->getIndex());

		// remove from map
		_targetMaterials.erase(_targetMaterials.find(matName));
	}

	void TerrainProjectionMarker::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		using namespace Ogre;

		// init projective decal
		// set up the main decal projection frustum
		_projectionFrustum = xNew0(Ogre::Frustum);
		_projectionFrustum->setProjectionType(Ogre::PT_ORTHOGRAPHIC);

		// load the images for the decal
		auto tex = TextureManager::getSingleton().getByName(GetTextureName(), "Materials");
		if(tex.isNull() || tex->isLoaded() == false)
			TextureManager::getSingleton().load(GetTextureName(), "Materials", TEX_TYPE_2D, 1);
	}

	void TerrainProjectionMarker::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		xDeleteSafe(_projectionFrustum);
	}

	void TerrainProjectionMarker::Spawn()
	{
		if( !_spawned )
		{
			SceneMarker::Spawn();
			_sceneNode->attachObject(_projectionFrustum);

			SetOrientation(Ogre::Quaternion(Ogre::Degree(90),Ogre::Vector3::UNIT_X));
			for(int i = 0; i < 4; ++i)
			{
				_lastProjectedTerrains[i] = 0;
			}
			_spawned = true;
		}
	}

	void TerrainProjectionMarker::Despawn()
	{
		if( _spawned )
		{
			_sceneNode->detachObject(_projectionFrustum);
			while( _targetMaterials.size() > 0 )
				RemoveMaterial(_targetMaterials.begin()->Key);
			SceneMarker::Despawn();
			_spawned = false;
		}
	}

	void TerrainProjectionMarker::UpdateProjection( SceneObject* thisObj )
	{
		// If owner != client player -> do nothing
		if(_owner != GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer())
			return;

		// All terrain materials used
		Array<string> neededMaterials;
		//FindProjectedTerrainMaterials(neededMaterials);

		if( neededMaterials.size() == 0)
		{
			// Nothing changed, return ( most probable situation )
			return;
		}

		// Remove duplicates
		for(unsigned int i = 0; i < neededMaterials.size(); ++i)
		{
			for(unsigned int k = i+1; k < neededMaterials.size(); ++k)
			{
				if( neededMaterials[i].compare(neededMaterials[k]) == 0 )
				{
					neededMaterials.removeAt(k);
					--k;
				}
			}
		}
		// Compare needed materials with ones already in use 
		auto usedMatIt = _targetMaterials.begin();
		while(true)
		{
			// stop if we are through
			if( usedMatIt == _targetMaterials.end() )
				break;

			// find in needed
			unsigned int matPos = 0;
			for(; matPos < neededMaterials.size(); ++matPos)
			{
				if( neededMaterials[matPos].compare(usedMatIt->Key) )
					break;
			}

			if( matPos == neededMaterials.size() )
			{
				// material is not needed any longer, so remove it
				const string& matName = usedMatIt->Key;
				++usedMatIt;
				RemoveMaterial(matName);
			}
			else
			{                
				// remove it from needed list, bedause it is already loaded
				neededMaterials.removeAt(matPos);
				++usedMatIt;
			}
		}

		// add all remaining needed to used list
		for(unsigned int i = 0; i < neededMaterials.size(); ++i)
		{
			AddMaterial(neededMaterials[i]);
		}
	}

	void TerrainProjectionMarker::UpdateSize(const Ogre::Vector2& size)
	{
		if( _halfSize * 2.f != size )
		{
			_halfSize = size / 2.f;

			// update aspect ratio
			_projectionFrustum->setAspectRatio(size.x/size.y);

			// update height
			_projectionFrustum->setOrthoWindowHeight(size.y);
			_projectionFrustum->setDebugDisplayEnabled(true);
			_projectionFrustum->setNearClipDistance(0.1f);
			_projectionFrustum->setFarClipDistance(2000.f);
		}
	}

	void TerrainProjectionMarker::FindProjectedTerrainMaterials(Array<string>& matNames)
	{
		//auto& pos = GetPositionAbsolute();
		//Vector3 orientedSize = Ogre::Quaternion(Ogre::Degree(-90),Ogre::Vector3::UNIT_X) * GetOrientationAbsolute() * Vector3(_halfSize.x, 0.f, _halfSize.y);

		//// Check which terrain cells are occupied by marker's corners 
		//// ( assume its smaller than terrain cell )
		//auto map = GameWorld::GlobalWorld->GetMap();
		//auto terrainGrid = GameWorld::GlobalWorld->GetMap()->GetTerrain()->GetTerrainGrid();
		//Ogre::Terrain* terrain;
		//uint32 xMin = (uint32)std::max(0.f, std::min(pos.x - orientedSize.x, pos.x + orientedSize.x));
		//uint32 xMax = (uint32)std::min(map->GetTerrainSize().x, std::max(pos.x - orientedSize.x, pos.x + orientedSize.x));
		//uint32 zMin = (uint32)std::max(0.f, std::max(pos.z - orientedSize.y, pos.z + orientedSize.y));
		//uint32 zMax = (uint32)std::min(map->GetTerrainSize().y, std::max(pos.z - orientedSize.z, pos.z + orientedSize.z));

		//// Check minimal corner terrain
		//terrain = terrainGrid->GetTerrainFromPosition(xMin, zMin);
		//if( _lastProjectedTerrains[0] != terrain )
		//{
		//	// Terrain cell changed ( or its 1st call )
		//	_lastProjectedTerrains[0] = terrain;
		//	matNames.push_back(terrain->getMaterialName());
		//}
		//// Check maximal corner terrain
		//terrain = terrainGrid->GetTerrainFromPosition(xMax, zMax);
		//if( _lastProjectedTerrains[0] != terrain )
		//{
		//	// Marker on multiple cells
		//	if( _lastProjectedTerrains[3] != terrain )
		//	{
		//		_lastProjectedTerrains[3] = terrain;
		//		matNames.push_back(terrain->getMaterialName());
		//	}
		//	terrain = terrainGrid->GetTerrainFromPosition(xMax, zMin);
		//	if( _lastProjectedTerrains[1] != terrain )
		//	{
		//		_lastProjectedTerrains[1] = terrain;
		//		matNames.push_back(terrain->getMaterialName());
		//	}
		//	terrain = terrainGrid->GetTerrainFromPosition(xMin, zMax);
		//	if( _lastProjectedTerrains[2] != terrain )
		//	{
		//		_lastProjectedTerrains[2] = terrain;
		//		matNames.push_back(terrain->getMaterialName());
		//	}
		//}
		//else
		//{
		//	// Max and min corner falls into same cell, so we'll have only one terrain
		//	_lastProjectedTerrains[1] = terrain;
		//	_lastProjectedTerrains[2] = terrain;
		//	_lastProjectedTerrains[3] = terrain;
	}
}
