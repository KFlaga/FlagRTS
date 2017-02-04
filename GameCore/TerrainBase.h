#pragma once

#include "TerrainInfo.h"
#include <Array2d.h>

namespace Ogre
{
	class Ray;
}

namespace FlagRTS
{
	class TerrainInfo;

	class TerrainBase
	{
	protected:
		Ogre::SceneManager* _ogreMgr;
		TerrainInfo _terrainInfo;
		
	public:
		TerrainBase(Ogre::SceneManager* ogreMgr) :
			_ogreMgr(ogreMgr)
		{ }

		virtual void Prepare(TerrainInfo& terrInfo) = 0;
		virtual void LoadAndSpawn() = 0;
		virtual void DespawnAndUnload() = 0;
		
		virtual Vector3 GetTerrainInterscetion(Ogre::Ray& ray) const = 0;
		virtual float GetTerrainHeight(float x, float z) const = 0;
		virtual float GetTerrainVertexHeight(uint32 vx, uint32 vy) const = 0;

		const TerrainInfo& GetTerrainInfo() const 
		{ 
			return _terrainInfo; 
		}

		virtual Array2d<float>& GetFinalHeightMap() = 0;
		virtual const Array2d<float>& GetFinalHeightMap() const = 0;	
	};
}