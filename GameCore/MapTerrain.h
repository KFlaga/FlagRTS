#pragma once

#include "DataTypes.h"
#include "OgreTerrainGrid.h"
#include <Xml.h>

namespace Ogre
{
	class SceneManager;
	class TerrainGlobalOptions;
	class TerrainGroup;
	class Terrain;
	class Light;
	class Image;
}

namespace FlagRTS
{
	class MapTerrain
	{
	private:
		TerrainInfo _terrainInfo;
		Ogre::TerrainGlobalOptions* _tgOptions;
		OgreTerrainGrid* _terrains; // Each terrain in group is cell ( such a way becouse Ogre handle only square terrains )
		Ogre::SceneManager* _ogreMgr;
		Ogre::Light* _globalLight;
		Vector2 _terrainBounds;

	public:
		MapTerrain(Ogre::SceneManager* ogreMgr, float cellSize, int tileCellSize);
		~MapTerrain();

		// Only stores info and prepares terrain to be created ( to load reseources )
		// Also reads and creates global light
		void ReadTerrainInfo(XmlNode* mapNode);

		// Loads terrain and tileset resources
		void LoadTerrain();

		// Gets global light
		Ogre::Light* GetGlobalLight() const { return _globalLight; }

		void UnloadTerrain();
		
		Vector3 GetTerrainInterscetion(Ogre::Ray ray) const { return _terrains->GetTerrainInterscetion(ray); }

		float GetTerrainHeight(float x, float z) const 
		{
			return _terrains->GetTerrainHeight(x,z);
		}

		float GetTerrainVertexHeight(uint32 vx, uint32 vy)
		{
			return _terrains->GetTerrainVertexHeight(vx,vy);
		}

		TerrainInfo& GetTerrainInfo() { return _terrainInfo; }

		OgreTerrainGrid* GetTerrainGrid() const { return _terrains; }

		const Vector2& GetTerrainSize() const
		{
			return _terrainBounds;
		}

		Array2d<float>& GetFinalHeightMap() { return _terrains->GetFinalHeightMap(); }
		const Array2d<float>& GetFinalHeightMap() const { return _terrains->GetFinalHeightMap(); }

	protected:
		// Creates global light ( ambient + directional ) with values read from node
		void CreateGlobalLight(XmlNode* lightNode);
		// Sets terrain global settings and creates new TerrainGrid
		void ConfigureTerrain();
		
	};
}