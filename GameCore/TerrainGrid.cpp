#include "TerrainGrid.h"
#include <OgreResourceGroupManager.h>
#include <OgreRay.h>
#include <Profiler.h>

namespace FlagRTS
{
	TerrainGrid::TerrainGrid(Ogre::SceneManager* ogreMgr) :
		_ogreMgr(ogreMgr),
		_bufferAlloc(xNew0(Ogre::Terrain::DefaultGpuBufferAllocator)),
		_finalHeightMap(1,1)
	{
		SetOriginAtTopLeft();
	}

	TerrainGrid::~TerrainGrid()
	{
		if(_terrains.size() > 0)
		{
			UnloadAllTerrain();
		}
		xDelete(_bufferAlloc);
	}

	Vector3 TerrainGrid::GetTerrainInterscetion(Ogre::Ray ray) const
	{
		for(int x = 0; x < _terrInfo.GetTilesX(); x++)
			for(int z = 0; z < _terrInfo.GetTilesZ(); z++)
			{
				auto result = _terrains[x,z]->rayIntersects(ray, false);
				if(result.first == true)
				{
					return result.second;
				}
			}
			return Vector3(-1,-1,-1);
	}

	void TerrainGrid::PrepareTerrains()
	{
		SetLayers();

		for(int z = 0; z < _terrInfo.GetTilesZ(); z++)
			for(int x = 0; x < _terrInfo.GetTilesX(); x++)
			{
				Ogre::Terrain* terrain = GetTerrain(x,z);
				_data.inputImage = GetHeightImage(x,z);
				_data.terrainSize = _data.inputImage->getWidth();

				// Compute correct tile center = center for topleft (0,0) + _origin
				_data.pos = Vector3( 
					_terrInfo.GetTileWorldSize()*((float)x+0.5f) + _origin.x, 0,
					_terrInfo.GetTileWorldSize()*((float)z+0.5f) + _origin.z);

				terrain->setResourceGroup(_resourceGroup);
				terrain->setGpuBufferAllocator(_bufferAlloc);

				terrain->prepare(_data);
			}

			SetNeighbours();
	}

	void TerrainGrid::SetLayers()
	{
		for (int i = 0; i < _terrInfo.GetTileset()->GetLoadedTexturesCount(); i++) 
		{
			Ogre::Terrain::LayerInstance layer;

			layer.worldSize = 160;
			layer.textureNames.push_back(
				_terrInfo.GetTileset()->GetTextureAt(i)->getName());
			_data.layerList.push_back(layer);
		}
	}

	void TerrainGrid::SetNeighbours()
	{
		// Set correct neightbours
		for(int z = 0; z < _terrInfo.GetTilesZ(); z++)
			for(int x = 0; x < _terrInfo.GetTilesX(); x++)
			{
				Ogre::Terrain* terrain = GetTerrain(x,z);
				if(x > 0) // have neighbours on left
				{
					terrain->setNeighbour(Ogre::Terrain::NEIGHBOUR_WEST, GetTerrain(x-1,z));
					if(z > 0) // have neighbour on top-left
						terrain->setNeighbour(Ogre::Terrain::NEIGHBOUR_NORTHWEST, GetTerrain(x-1,z-1));
					if(z < _terrInfo.GetTilesZ() - 1) // have neighnour on bot-left
						terrain->setNeighbour(Ogre::Terrain::NEIGHBOUR_SOUTHWEST, GetTerrain(x-1,z+1));
				}
				if( x < _terrInfo.GetTilesX() - 1) // have neighbours on right
				{
					terrain->setNeighbour(Ogre::Terrain::NEIGHBOUR_EAST, GetTerrain(x+1,z));
					if(z > 0) // have neighbour on top-right
						terrain->setNeighbour(Ogre::Terrain::NEIGHBOUR_NORTHEAST, GetTerrain(x+1,z-1));
					if(z < _terrInfo.GetTilesZ() - 1) // have neighnour on bot-right
						terrain->setNeighbour(Ogre::Terrain::NEIGHBOUR_SOUTHEAST, GetTerrain(x+1,z+1));
				}
				if(z > 0) // have neighbour on top
					terrain->setNeighbour(Ogre::Terrain::NEIGHBOUR_NORTH, GetTerrain(x,z-1));
				if(z < _terrInfo.GetTilesZ() - 1) // have neighnour on bot
					terrain->setNeighbour(Ogre::Terrain::NEIGHBOUR_SOUTH, GetTerrain(x,z+1));
			}
	}

	void TerrainGrid::LoadTerrain(uint32 x, uint32 z)
	{
		Ogre::Terrain* terrain = GetTerrain(x,z);

		terrain->load(0,true);
	}

	void TerrainGrid::LoadAllTerrains()
	{
		for(int z = 0; z < _terrInfo.GetTilesZ(); z++)
			for(int x = 0; x < _terrInfo.GetTilesX(); x++)
			{
				LoadTerrain(x,z);
			}

			ComputeHeightMap();
	}

	void TerrainGrid::SetTerrainInfo(const TerrainInfo& tinfo)
	{
		_terrInfo = tinfo;

		_data.worldSize = _terrInfo.GetTileWorldSize();
		_data.inputScale = (float)_terrInfo.GetHeightScale();
		_data.inputBias = (float)_terrInfo.GetBaseHeight();
		_data.maxBatchSize = 65;
		_data.minBatchSize = 33;
		_data.terrainAlign = Ogre::Terrain::ALIGN_X_Z;

		_data.deleteInputData = false;

		if(_terrains.size() > 0) { } // Delete old terrains

		for(int z = 0; z < _terrInfo.GetTilesZ(); z++)
			for(int x = 0; x < _terrInfo.GetTilesX(); x++)
			{
				Ogre::Terrain* terrain = OGRE_NEW Ogre::Terrain(_ogreMgr);
				_terrains.push_back(terrain);
				terrain->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAIN - 10);
			}

			_heightMaps.resize(_terrains.size());
	}

	void TerrainGrid::UnloadAllTerrain()
	{
		for(uint32 i = 0; i < _terrains.size(); i++)
		{
			_terrains[i]->unload();
			//	OGRE_DELETE _terrains[i];
		}
		_terrains.clear();
		for(uint32 i = 0; i < _heightMaps.size(); i++)
		{
			OGRE_DELETE _heightMaps[i];
		}
		_heightMaps.clear();
	}

	bool TerrainGrid::LoadTerrainFromFileIfExists()
	{
		// File names convention : mapname_terrain_'x'_'z'.trr
		string baseName = _terrInfo.GetName() + "_terrain_";
		Array<string> fileNames;
		for(int z = 0; z < _terrInfo.GetTilesZ(); z++)
			for(int x = 0; x < _terrInfo.GetTilesX(); x++)
			{
				string finalName = baseName + std::to_string(x) + "_" + std::to_string(z);
				// Check if file exists
				auto fileList = Ogre::ResourceGroupManager::getSingleton().
					findResourceFileInfo("Maps", finalName, true);

				if(fileList->size() != 1) // File not found or is ambigous
					return false;

				fileNames.push_back(fileList->at(0).path + fileList->at(0).filename);
			}

			// All terrains found - load them
			for(int z = 0; z < _terrInfo.GetTilesZ(); z++)
				for(int x = 0; x < _terrInfo.GetTilesX(); x++)
				{
					GetTerrain(x,z)->load(fileNames[GetVectorPos(x,z)]);
				}

				return true;
	}

	void TerrainGrid::SaveTerrainToFile()
	{
		auto pathList = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList("Maps");

		// Look for path to directory with terrains ( in group Maps )
		string terrainsPath = "";
		auto pathIt = pathList.begin();
		for(; pathIt != pathList.end(); pathIt++)
		{
			if((*pathIt)->archive->getName().find("Terrain") != string::npos)
			{
				terrainsPath = (*pathIt)->archive->getName();
			}
		}
		// If terrain path was found save all terrains in files : 'mapname'_terrain_'x'_'z'.trr
		if(terrainsPath.size() > 0)
		{
			string baseName = terrainsPath + "\\" +_terrInfo.GetName() + "_terrain_";
			for(int z = 0; z < _terrInfo.GetTilesZ(); z++)
				for(int x = 0; x < _terrInfo.GetTilesX(); x++)
				{
					string finalName = baseName + std::to_string(x) + "_" + std::to_string(z) + ".trr";
					GetTerrain(x,z)->save(finalName);
				}
		}
	}

	void TerrainGrid::FreeTemporaryResources()
	{
		for(int z = 0; z < _terrInfo.GetTilesZ(); z++)
			for(int x = 0; x < _terrInfo.GetTilesX(); x++)
			{
				GetTerrain(x,z)->freeTemporaryResources();
			}
	}

	inline Vector3 GetPointCoords(const int x, const int z, 
		const float cellSize, Array2d<float>& hMap)
	{
		return Vector3(x*cellSize, hMap(z, x), z*cellSize);
	}

	float TerrainGrid::GetTerrainHeight(float x, float z)
	{
		PROFILE_REGISTER_CLOCK(pclock, "GetTerrrainHeight");
		PROFILE_START(pclock);
		x = std::max(0.f, x);
		z = std::max(0.f, z);
		x = std::min(_terrInfo.GetTileWorldSize() * _terrInfo.GetTilesCount().X, x);
		z = std::min(_terrInfo.GetTileWorldSize() * _terrInfo.GetTilesCount().Y, z);
		int leftCell = (int)(x / _terrInfo.GetCellSize());
		int topCell = (int)(z / _terrInfo.GetCellSize());

		// 3 vertices creating triangle point (x,z) is over
		Vector3 v1,v2,v3;
		// According to OGRE:
		/* For even / odd tri strip rows, triangles are this shape:
		even     odd
		3---2   3---2
		| / |   | \ |
		0---1   0---1
		*/
		// In here even/odd order is reversed
		// So we need to find those vertices
		if((topCell & 1) != 0)
		{
			// EVEN IN OGRE
			v1 = GetPointCoords(leftCell, topCell+1, _terrInfo.GetCellSize(), _finalHeightMap);
			v2 = GetPointCoords(leftCell+1, topCell, _terrInfo.GetCellSize(), _finalHeightMap);

			// Find triangle the point is over ( top-left or bot-right ) : dividing line have equation x+y=csize
			if( x - leftCell * _terrInfo.GetCellSize() + 
				z - topCell * _terrInfo.GetCellSize() > 
				_terrInfo.GetCellSize())
			{
				// Point is over bottom triangle
				v3 = GetPointCoords(leftCell+1, topCell+1, _terrInfo.GetCellSize(), _finalHeightMap);
			}
			else
			{
				// Point is over top triangle
				v3 = GetPointCoords(leftCell, topCell, _terrInfo.GetCellSize(), _finalHeightMap);
			}
		}
		else
		{
			// ODD IN OGRE
			v1 = GetPointCoords(leftCell, topCell, _terrInfo.GetCellSize(), _finalHeightMap);
			v2 = GetPointCoords(leftCell+1, topCell+1, _terrInfo.GetCellSize(), _finalHeightMap);

			// Find triangle the point is over ( bot-left or top-right ) : dividing line have equation x-y=0
			if( x - leftCell * _terrInfo.GetCellSize() + 
				z - topCell * _terrInfo.GetCellSize() < 0.f)
			{
				// Point is over bottom-left triangle
				v3 = GetPointCoords(leftCell, topCell+1, _terrInfo.GetCellSize(), _finalHeightMap);
			}
			else
			{
				// Point is over top-right triangle
				v3 = GetPointCoords(leftCell+1, topCell, _terrInfo.GetCellSize(), _finalHeightMap);
			}
		}

		// Find normal to triangle plane
		Vector3 p1(v1 - v3);
		Vector3 p2(v2 - v3);
		Vector3 n(p1.y*p2.z - p1.z*p2.y, p1.z*p2.x - p1.x*p2.z, p1.x*p2.y - p1.y*p2.x);
		// Find D in Ax+By+Cz+D = 0
		float D = -(n.x * v1.x + n.y*v1.y + n.z*v1.z);
		// Finnally can find height of point of interest
		float h = -(n.x*x+n.z*z+D)/n.y;
		PROFILE_END(pclock);
		return h;
	}

	float TerrainGrid::GetTerrainVertexHeight(uint32 vx, uint32 vy)
	{
		return _finalHeightMap(vy, vx);
	}

	void TerrainGrid::ComputeHeightMap()
	{
		new (&_finalHeightMap) Array2d<float>(
			_terrInfo.GetTilesZ() * _terrInfo.GetTileCellSize()+1,
			_terrInfo.GetTilesX() * _terrInfo.GetTileCellSize()+1);

		for(int tz = 0; tz < _terrInfo.GetTilesZ(); tz++)
			for(int tx = 0; tx < _terrInfo.GetTilesX(); tx++)
			{
				Ogre::Terrain* terrain = GetTerrain(tx, tz);

				for(uint32 vz = 0; vz < _terrInfo.GetTileCellSize(); ++vz)
				{
					for(uint32 vx = 0; vx < _terrInfo.GetTileCellSize(); ++vx)
					{
						// Terrain is in inverted z coord, so get size-z point
						float h = terrain->getHeightAtPoint(vx,_terrInfo.GetTileCellSize()-vz);
						_finalHeightMap.SetAt(
							tz * _terrInfo.GetTileCellSize() + vz,
							tx * _terrInfo.GetTileCellSize() + vx,
							h);
					}
				}
			}
	}
}