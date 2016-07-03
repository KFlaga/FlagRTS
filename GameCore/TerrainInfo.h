#pragma once

#include "DataTypes.h"
#include "Tileset.h"

namespace FlagRTS
{
	// Contains info on map terrain
	class TerrainInfo
	{
	private:
		string _name;
		IntVector2 _tilesCount;
		int _tileCellSize;
		float _cellSize;
		float _tileWorldSize;
		int _baseHeight;
		int _heightScale;
		RefPtr<Tileset> _tileset;

	public:
		TerrainInfo() { }

		void SetName(const string& name) { _name = name; }
		const string& GetName() const { return _name; }
		
		void SetCellSize(float size) { _cellSize = size; }
		float GetCellSize() const { return _cellSize; }
		
		void SetTileCellSize(int size) { _tileCellSize = size; }
		int GetTileCellSize() const { return _tileCellSize; }

		void SetTileWorldSize(float size) { _tileWorldSize = size; }
		float GetTileWorldSize() const { return _tileWorldSize; }

		void SetTilesX(int cellsX) { _tilesCount.X = cellsX; }
		int GetTilesX() const { return _tilesCount.X; }

		void SetTilesZ(int cellsZ) { _tilesCount.Y = cellsZ; }
		int GetTilesZ() const { return _tilesCount.X; }

		void SetTilesCount(const IntVector2& counts) { _tilesCount = counts; }
		const IntVector2& GetTilesCount() const { return _tilesCount; }

		void SetBaseHeight(int height) { _baseHeight = height; }
		int GetBaseHeight() const { return _baseHeight; }

		void SetHeightScale(int scale) { _heightScale = scale; }
		int GetHeightScale() const { return _heightScale; }

		void SetTileset(const RefPtr<Tileset>& tileset) { _tileset = tileset; }
		RefPtr<Tileset> GetTileset() const { return _tileset; }
	};
}