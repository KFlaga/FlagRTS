#include "ArtefixMapTerrainLoader.h"
#include "TerrainGrid.h"

#include <Ogre\Terrain\OgreTerrain.h>
#include <Ogre\Terrain\OgreTerrainGroup.h>

namespace FlagRTS
{
	ArtefixTerrainLoader::ArtefixTerrainLoader(TerrainGrid* terrain, TerrainInfo& terrInfo) :
		ITerrainLoader(terrain, terrInfo) 
	{

	}

	void ArtefixTerrainLoader::LoadTerrain()
	{
		for(int x = 0; x < _terrInfo.GetTilesX(); x++)
			for(int z = 0; z < _terrInfo.GetTilesZ(); z++)
				DefineTerrain(x,z);

		
		_terrains->PrepareTerrains();
		_terrains->LoadAllTerrains();

		for(int x = 0; x < _terrInfo.GetTilesX(); x++)
			for(int z = 0; z < _terrInfo.GetTilesZ(); z++)
			{
				InitBlendMaps(x,z);
			}

	}

	void ArtefixTerrainLoader::UnloadTerrain()
	{

	}


	void ArtefixTerrainLoader::LoadTerrainHeightMap(const string& mapname, uint8 x, uint8 z, Ogre::Image* img)
	{
		string finalname = mapname + "_hm_" + std::to_string(x) + "_" + std::to_string(z) + ".png";
		img->load(finalname,"Maps");
	}

	void ArtefixTerrainLoader::DefineTerrain(int x, int y)
	{
		Ogre::Image* img = OGRE_NEW Ogre::Image();
		LoadTerrainHeightMap(_terrInfo.GetName() , x , y , img);
		_terrains->SetHeightImage(x,y,img);
	}

	void ArtefixTerrainLoader::InitBlendMaps(int x, int z)
	{
		// NUm of layers = num of textures
		int maxLayer = _terrInfo.GetTileset()->GetLoadedTexturesCount();

		// Load artefix blend maps
		Ogre::Image blendMapImage[3];
		string bmBaseName = _terrInfo.GetName() + "_bm_";

		for(int i = 0; i < 3; i++)
		{
			blendMapImage[i].load(bmBaseName + std::to_string(x) + "_" + std::to_string(z) + "_" + std::to_string(i) + ".png" , "Maps");		
		}
		int blendImageNumber = 0;
		Ogre::Terrain* terrain = _terrains->GetTerrain(x,z);
		int layers = terrain->getLayerCount();

		// Converting ETM blendmaps to Ogre::Terrain blendmaps

		for(int i = 1; i < maxLayer; i++)
		{
			// Get terrain blend map data
			Ogre::TerrainLayerBlendMap *blendMap;
			float *blendData;

			blendMap = terrain->getLayerBlendMap(i);
			// Get pointer to [0,0] of blend map : row major 2d array
			blendData = blendMap->getBlendPointer();        

			// For each point in Ogre blendmap find corresponding value in ETM
			for(int y = 0; y < blendMapImage->getHeight(); y++)
			{
				for(int x = 0; x < blendMapImage->getWidth(); x++)
				{
					// TODO scale (x,y) so that input blendMapImage fits blendMap ( as they may have different sizes )
					Ogre::ColourValue blendColour = blendMapImage[blendImageNumber].getColourAt(x, y, 0);
					switch(i-1)
					{					
					case 0: case 3: case 6: // Textures nr 1,4,7 are on 1st,2nd,3rd ETM blend map as red 
						{	*blendData = blendColour.r;
							break;
						}					
					case 1: case 4: case 7: // Textures nr 2,5,8 are on 1st,2nd,3rd ETM blend map as red 
						{
							*blendData = blendColour.g;
							break;
						}
					case 2: case 5: case 8: // Textures nr 3,6,9 are on 1st,2nd,3rd ETM blend map as red 
						{
							*blendData = blendColour.b;
							break;
						}			
					}               
					blendData++;
				}
			}
			// Update whole blendmap
			blendMap->dirty(); 
			blendMap->update();

			if ( i % 3 == 0 ) 
				blendImageNumber++;

			// save converted blendmaps
			//if (saveBlendMaps) saveConvertedBlendMap(i,blendMap);
		}  
	}
}