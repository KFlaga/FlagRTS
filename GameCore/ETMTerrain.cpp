#include "ETMTerrain.h"
#include <ETM\ETTerrainManager.h>
#include <ETM\ETTerrainInfo.h>

#include <RenderSystemManager.h>
#include "GameWorld.h"

namespace FlagRTS
{
	ETTerrain::ETTerrain(Ogre::SceneManager* ogreMgr) :
		TerrainBase(ogreMgr),
		_etmTerrainMgr(0)
	{

	}

	ETTerrain::~ETTerrain()
	{
		DespawnAndUnload();
	}

	void ETTerrain::Prepare(TerrainInfo& terrInfo)
	{
		_terrainInfo = terrInfo;

		// Create ETTerrainManager
		// Height of viewport if fixed 1000 - it influences only LOD changing, so
		// some sensible value should be ok for most resolutions
		_etmTerrainMgr = xNew0(ET::TerrainManager _ogreMgr);
		_etmTerrainMgr->setLODErrorMargin(3, 1000);
		_etmTerrainMgr->setUseLODMorphing(false, 0.2, "morphFactor");
		
		// Set default ETTerrain material
		Ogre::MaterialPtr material (Ogre::MaterialManager::getSingleton().
			getByName("ETTerrainMaterial"));
		_etmTerrainMgr->setMaterial(material);
	}

	void ETTerrain::LoadAndSpawn()
	{


		// Fill 
	}

	void ETTerrain::DespawnAndUnload()
	{
		if(_etmTerrainMgr != 0)
		{
			_etmTerrainMgr->destroyTerrain();
			xDelete(_etmTerrainMgr);
			_etmTerrainMgr = 0;
		}
	}

	void ETTerrainMaterialGenerator::CreateTerrainMaterial(TerrainInfo& terrInfo, 
		const string& matName, const string& groupName)
	{
		// Load base material
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(matName, groupName);
		
		Ogre::Technique* technique = material->createTechnique();

		Ogre::Pass* splatPass = technique->createPass();
		
	}
}