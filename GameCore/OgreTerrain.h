#pragma once

#include "TerrainBase.h"
#include "OgreTerrainGrid.h"
#include <Xml.h>

namespace FlagRTS
{
	class OgreTerrain : public TerrainBase
	{
	protected:
		Ogre::TerrainGlobalOptions* _tgOptions;
		OgreTerrainGrid* _terrainGrid; // Each terrain in group is cell ( such a way becouse Ogre handle only square terrains )
		
	public:
		OgreTerrain(Ogre::SceneManager* ogreMgr);
		~OgreTerrain();


		void Prepare(TerrainInfo& terrInfo);

		void LoadAndSpawn();

		void DespawnAndUnload();

		Vector3 GetTerrainInterscetion(Ogre::Ray& ray) const
		{
			return _terrainGrid->GetTerrainInterscetion(ray);
		}

		float GetTerrainHeight(float x, float z) const
		{
			return _terrainGrid->GetTerrainHeight(x,z);
		}

		float GetTerrainVertexHeight(uint32 vx, uint32 vy) const
		{
			return _terrainGrid->GetTerrainVertexHeight(vx,vy);
		}

		Array2d<float>& GetFinalHeightMap()
		{
			return _terrainGrid->GetFinalHeightMap();
		}

		const Array2d<float>& GetFinalHeightMap() const
		{
			return _terrainGrid->GetFinalHeightMap();
		}
	};
}