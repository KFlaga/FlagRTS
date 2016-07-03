#pragma once

#include "DataTypes.h"
#include <Array.h>
#include <IntVector2.h>


namespace FlagRTS
{
	class CellColorer
	{
	private:
		Ogre::SceneManager* _ogreMgr;
		Ogre::SceneNode* _sceneNode;
		Array<IntVector2> _cells;
		Ogre::ManualObject* _renderCells;
		Vector2 _cellSize;
		Vector2 _maxPoint;
		Vector2 _minPoint;
		string _matName;
		bool _rendered;

	public:
		CellColorer(Ogre::SceneManager* ogreMgr, const string& materialName);
		~CellColorer();

		void AddCell(const IntVector2& cell);
		void AddCell(int x, int y)
		{
			AddCell(IntVector2(x,y));
		}

		/*void RemoveCell(const IntVector2& cell);
		void RemoveCell(int x, int y)
		{
			RemoveCell(IntVector2(x,y));
		}*/

		void AttachToScene(Ogre::SceneNode* rootNode);
		void DetachFromScene();
		void ClearRender();
		void ClearCells();
		void Draw();

		void SetCellSize(const Vector2& cellSize)
		{
			_cellSize = cellSize;
		}
	};
}