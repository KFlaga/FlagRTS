#pragma once

#include <Ogre\Terrain\OgreTerrain.h>
#include "DataTypes.h"
#include "TerrainInfo.h"
#include <Array2d.h>

namespace FlagRTS
{
	// Works similar to Ogre::TerrainGroup, but for some reason it
	// doesn't work properly
	// More specialzed than TerrainGroup, so not all properties need to
	// be set manualy
	class TerrainGrid
	{
	private:
		Ogre::SceneManager* _ogreMgr;
		Ogre::Terrain::ImportData _data;

		Array<Ogre::Terrain*> _terrains; // All terrains stored in row-major order : (x,z) = z*cellsX + x
		Array<Ogre::Image*> _heightMaps;
		Array2d<float> _finalHeightMap;
		string _resourceGroup;
		Ogre::Terrain::GpuBufferAllocator* _bufferAlloc;

		// Global info on created terrains
		TerrainInfo _terrInfo;
		Vector3 _center;
		Vector3 _origin;

	public:
		TerrainGrid(Ogre::SceneManager*);
		~TerrainGrid();

		uint32 GetVectorPos(uint32 x, uint32 z) const 
		{ 
			//_ASSERT(z*_terrInfo.GetCellsX() + x < _terrains.size());
			return std::min(z*_terrInfo.GetTilesX() + x, _terrains.size()-1); 
		}

		// Sets center such that top-left corner is in point (0,0)
		void SetOriginAtTopLeft() 
		{ 
			SetTerrainsCenter(Vector3(
			(float)_terrInfo.GetTilesX()*_terrInfo.GetTileWorldSize()*0.5f, 0,
			(float)_terrInfo.GetTilesZ()*_terrInfo.GetTileWorldSize()*0.5f));
		}

		void SetTerrainsCenter(const Vector3& center) 
		{ 
			_center = center; 
			_origin = Vector3(_center.x - _terrInfo.GetTilesX()*_terrInfo.GetTileWorldSize()*0.5f, 0,
				_center.z - _terrInfo.GetTilesZ()*_terrInfo.GetTileWorldSize()*0.5f);
		}
		const Vector3& GetTerrainsCenter() const { return _center; }

		const string& GetResourceGroup() const { return _resourceGroup; } 
		void SetResourceGroup(const string& groupName) { _resourceGroup = groupName; }

		Ogre::Image* GetHeightImage(uint32 x, uint32 z) const { return _heightMaps[GetVectorPos(x,z)]; }
		void SetHeightImage(uint32 x, uint32 z, Ogre::Image* img) { _heightMaps[GetVectorPos(x,z)] = img; }
		
		Ogre::Terrain* GetTerrain(uint32 x, uint32 z)
		{
			return _terrains[GetVectorPos(x,z)];
		}

		Ogre::Terrain* GetTerrainFromPosition(float x, float z)
		{
			return GetTerrain((uint32)(x/_terrInfo.GetTileWorldSize()), 
				(uint32)(z/_terrInfo.GetTileWorldSize()));
		}

		void PrepareTerrains();

		void LoadTerrain(uint32 x, uint32 z);
		// Loads terrains data and computes height map
		void LoadAllTerrains();

		void UnloadAllTerrain();

		// Sets terrain info and updates terrain data
		void SetTerrainInfo(const TerrainInfo& tinfo);
		// Return copy of terrain info
		TerrainInfo GetTerrainInfo() const { return _terrInfo; }

		void FreeTemporaryResources();

		// Loads terrain from file if all terrains exists ( names created based on
		// TerrainInfo, so it must be set earlier )
		// Returns true if terrains was loaded or false otherwise
		bool LoadTerrainFromFileIfExists();

		// Saves fully loaded terrain to file
		void SaveTerrainToFile();

		// Compute point of intersection of given ray with terrain
		Vector3 GetTerrainInterscetion(Ogre::Ray ray) const;

		// Returns height of terrain in given point -> weighted average of 4 closest vertices
		float GetTerrainHeight(float x, float z);
		// Returns height of terrain in one of vertices
		float GetTerrainVertexHeight(uint32 vx, uint32 vy);

		Array2d<float>& GetFinalHeightMap() { return _finalHeightMap; }
		const Array2d<float>& GetFinalHeightMap() const { return _finalHeightMap; }

	protected:
		void SetNeighbours();
		void SetLayers();
		void ComputeHeightMap();
	};
}