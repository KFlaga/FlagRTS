#include "CellColorer.h"
#include "GameWorld.h"
#include "Map.h"

#include <Ogre\OgreSceneManager.h>
#include <OgreManualObject.h>
#include <Ogre\OgreResourceGroupManager.h>
#include <Ogre\OgreMaterialManager.h>
#include <Ogre\OgreMaterial.h>


namespace FlagRTS
{
	CellColorer::CellColorer(Ogre::SceneManager* ogreMgr, const string& matName) :
		_ogreMgr(ogreMgr),
		_matName(matName),
		_sceneNode(0)
	{
		_cellSize = GameWorld::GlobalWorld->GetMap()->GetWorldSizeOfCell();
		_renderCells = _ogreMgr->createManualObject();
		_renderCells->setCastShadows(false);
		_renderCells->setDynamic(true);
		_renderCells->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAIN);
	}

	CellColorer::~CellColorer()
	{
		ClearRender();
		_ogreMgr->destroyManualObject(_renderCells);
	}

	void CellColorer::AddCell(const IntVector2& cell)
	{
		_cells.push_back(cell);
		_maxPoint.x = std::max(_maxPoint.x, cell.X * _cellSize.x);
		_maxPoint.y = std::max(_maxPoint.y, cell.Y * _cellSize.y);
		_minPoint.x = std::min(_minPoint.x, cell.X * _cellSize.x);
		_minPoint.y = std::min(_minPoint.y, cell.Y * _cellSize.y);
	}

	/*void CellColorer::RemoveCell(const IntVector2& cell)
	{
		for(uint32 i = 0; i < _cells.size(); ++i)
		{
			if(_cells[i] == cell )
			{
				_cells.removeAt(i);
				break;
			}
		}
	}*/

	void CellColorer::AttachToScene(Ogre::SceneNode* rootNode)
	{
		DetachFromScene();
		_sceneNode = rootNode->createChildSceneNode();
		_sceneNode->attachObject(_renderCells);
	}

	void CellColorer::DetachFromScene()
	{
		if(_sceneNode != 0)
		{
			_sceneNode->detachAllObjects();
			Ogre::SceneNode* parentNode = _sceneNode->getParentSceneNode();
			parentNode->removeAndDestroyChild(_sceneNode->getName());
			_sceneNode = 0;
		}
	}

	void CellColorer::ClearRender()
	{
		if (_rendered)
		{
			_renderCells->setVisible(false);
			_renderCells->clear();
			_rendered = false;
		}
	}

	void CellColorer::ClearCells()
	{
		_cells.clear();
		_maxPoint = Vector2(0.f,0.f);
		_minPoint = Vector2(1e12f,1e12f);
	}

	void CellColorer::Draw()
	{
		if (_rendered || (_cells.empty()) )
			return;
		else
			_rendered = true;

		_renderCells->estimateVertexCount(_cells.size() * 4);
		_renderCells->estimateIndexCount(_cells.size() * 6);
		_renderCells->begin(_matName, Ogre::RenderOperation::OT_TRIANGLE_LIST);

		Vector2 center = (_maxPoint + _minPoint + _cellSize) * 0.5f;
		uint32 cellIdx = 0;
		for (uint32 cell = 0; cell < _cells.size(); ++cell)
		{ 
			Vector2 corner(_cells[cell].X * _cellSize.x, _cells[cell].Y * _cellSize.y);

			_renderCells->position(
				Vector3(corner.x,
				GameWorld::GlobalWorld->GetMap()->GetTerrainHeight(corner.x, corner.y) + 1.f,
				corner.y));

			_renderCells->position(
				Vector3(corner.x + _cellSize.x,
				GameWorld::GlobalWorld->GetMap()->GetTerrainHeight((corner.x + _cellSize.x), corner.y) + 1.f,
				corner.y));

			_renderCells->position(
				Vector3(corner.x,
				GameWorld::GlobalWorld->GetMap()->GetTerrainHeight(corner.x, (corner.y + _cellSize.y)) + 1.f,
				corner.y + _cellSize.y));

			_renderCells->position(
				Vector3(corner.x + _cellSize.x,
				GameWorld::GlobalWorld->GetMap()->GetTerrainHeight((corner.x + _cellSize.x), (corner.y + _cellSize.y)) + 1.f,
				corner.y + _cellSize.y));

			_renderCells->triangle(cellIdx, cellIdx+3, cellIdx+1);
			_renderCells->triangle(cellIdx+3, cellIdx, cellIdx+2);
			cellIdx += 4;
		}

		_renderCells->end();
		_renderCells->setVisible(true);
	}
}