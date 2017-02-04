#pragma once

#include "DataTypes.h"
#include "Tileset.h"
#include <Xml.h>

namespace Ogre
{
	class Light;
}

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
		Vector2 _worldSize;
		Vector3 _worldOffset;
		RefPtr<Tileset> _tileset;
		Ogre::Light* _globalLight;

	public:
		TerrainInfo();
		TerrainInfo(float cellSize, int cellsInTile);

		void ReadFromXml(XmlNode* terrainInfoNode);

		void SetName(const string& name) { _name = name; }
		const string& GetName() const { return _name; }
		
		void SetWorldSizeOfCell(float size) { _cellSize = size; }
		float GetWorldSizeOfCell() const { return _cellSize; }
		
		void SetCellCountInTile(int size) { _tileCellSize = size; }
		int GetCellCountInTile() const { return _tileCellSize; }

		void SetWorldSizeOfTile(float size) { _tileWorldSize = size; }
		float GetWorldSizeOfTile() const { return _tileWorldSize; }

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
		
		void SetWorldSize(const Vector2& v) { _worldSize = v; }
		const Vector2& GetWorldSize() const { return _worldSize; }
		
		void SetWorldOffset(const Vector3& v) { _worldOffset = v; }
		const Vector3& GetWorldOffset() const { return _worldOffset; }
	
		void SetGlobalLight(Ogre::Light* light) { _globalLight = light; }
		Ogre::Light* GetGlobalLight() const { return _globalLight; }
	};
}