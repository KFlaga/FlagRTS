#pragma once

#include "TerrainBase.h"

namespace ET
{
	class TerrainManager;
}

namespace FlagRTS
{
	class ETTerrain : public TerrainBase
	{
	private:
		ET::TerrainManager* _etmTerrainMgr;

	public:
		ETTerrain(Ogre::SceneManager*);
		~ETTerrain();
		
		void Prepare(TerrainInfo& terrInfo);
		void LoadAndSpawn();
		void DespawnAndUnload();
		
		Vector3 GetTerrainInterscetion(Ogre::Ray& ray) const;
		float GetTerrainHeight(float x, float z) const;
		float GetTerrainVertexHeight(uint32 vx, uint32 vy) const;

		Array2d<float>& GetFinalHeightMap();
		const Array2d<float>& GetFinalHeightMap() const;	
	};

	class ETTerrainMaterialGenerator
	{
	public:
		void CreateTerrainMaterial(TerrainInfo& terrInfo, const string& matName, const string& matGroup);
	};
}