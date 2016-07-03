#pragma once

#include <Xml.h>
#include "TerrainInfo.h"

namespace FlagRTS
{
	class Tileset;
	class TerrainGrid;

	// Interface for loaders of MapTerrain differenting on data which to load from
	//
	class ITerrainLoader
	{
	protected:
		TerrainInfo& _terrInfo;
		TerrainGrid* _terrains;

	public:
		// Gets configured TerrainGroup
		ITerrainLoader(TerrainGrid* terrains, TerrainInfo& terrInfo) :
			_terrInfo(terrInfo), 
			_terrains(terrains)
		{
		}

		virtual void LoadTerrain() = 0;

		virtual void UnloadTerrain() = 0;

		void SetTerrainInfo(TerrainInfo& terrInfo) { _terrInfo = terrInfo; }

		TerrainGrid* GetTerrainGrid() const { return _terrains; }

	};
}