#include "OgreTerrain.h"

#include <Ogre\Terrain\OgreTerrain.h>
#include <Ogre\Terrain\OgreTerrainGroup.h>
#include <OgreImage.h>
#include "OgreTerrainLoader.h"

namespace FlagRTS
{
	OgreTerrain::OgreTerrain(Ogre::SceneManager* ogreMgr) :
		TerrainBase(ogreMgr),
		_tgOptions(0), 
		_terrainGrid(0)
	{
		_terrainInfo.SetBaseHeight(0);
		//_terrainInfo.SetWorldSizeOfCell(cellSize);
		//_terrainInfo.SetCellCountInTile(tileCellSize);
		//_terrainInfo.SetWorldSizeOfTile((float)_terrainInfo.GetCellCountInTile()*cellSize);
		_terrainInfo.SetTilesX(1);
		_terrainInfo.SetTilesZ(1);
		_terrainInfo.SetHeightScale(200);
		_terrainInfo.SetName("TestMap");
	}

	OgreTerrain::~OgreTerrain()
	{
		DespawnAndUnload();
	}

	void OgreTerrain::Prepare(TerrainInfo& terrInfo)
	{
		_terrainInfo = terrInfo;
		
		_tgOptions = OGRE_NEW Ogre::TerrainGlobalOptions();

		_tgOptions->setCastsDynamicShadows(false);
		_tgOptions->setCompositeMapDistance(3000);
		_tgOptions->setLightMapDirection(terrInfo.GetGlobalLight()->getDerivedDirection());
		_tgOptions->setCompositeMapAmbient(_ogreMgr->getAmbientLight());
		_tgOptions->setCompositeMapDiffuse(terrInfo.GetGlobalLight()->getDiffuseColour());

		_terrainGrid = xNew1(OgreTerrainGrid, _ogreMgr);

		_terrainGrid->SetResourceGroup("Maps");
		_terrainGrid->SetTerrainInfo(_terrainInfo);
	}

	void OgreTerrain::LoadAndSpawn()
	{
		_terrainInfo.GetTileset()->LoadTileset();

		// Find if info on terrain is saved. If it is load from file
		// otherwise perform manual load and save it for later reuse
		// FOR NOW DO NOT LOAD FROM FILE, AS IT TAKES MORE TIME THAN CREATING NEW ONE (at least in debug)

		//if(!_terrains->LoadTerrainFromFileIfExists())
		//{
			OgreTerrainLoader terrainLoader(_terrainGrid, &_terrainInfo);
			terrainLoader.LoadTerrain();
			//_terrains->SaveTerrainToFile();
		//}

		_terrainGrid->FreeTemporaryResources();
	}

	void OgreTerrain::DespawnAndUnload()
	{
		if(_tgOptions != 0)
		{
			OGRE_DELETE _tgOptions;
			_tgOptions = 0;
		}
		if(_terrainGrid != 0)
		{
			_terrainGrid->UnloadAllTerrain();
			_terrainGrid = 0;
		}
	}
}