#include "PathingDrawer.h"
#include <Ogre\OgreManualObject.h>
#include <Ogre\OgreRenderOperation.h>
#include <Ogre\OgreResourceGroupManager.h>
#include <Ogre\OgreMaterialManager.h>
#include <Ogre\OgreMaterial.h>
#include <Ogre\OgreTechnique.h>
#include <Ogre\OgreHardwareBufferManager.h>
#include <Ogre\OgreSceneManager.h>
#include <Ogre\OgreSceneNode.h>
#include <Memory.h>
#include "GameWorld.h"
#include "Map.h"

namespace FlagRTS
{
	PathingDrawer::PathingDrawer(Ogre::SceneManager* ogreMgr, const Vector3& color, const string& name) :
		_ogreMgr(ogreMgr),
		_renderedLines(0),
		_debugNode(0),
		_linesDrawn(false),
		_name(name)
	{
		Ogre::StringVector resourceGroups = Ogre::ResourceGroupManager::
			getSingletonPtr()->getResourceGroups();

		// Find if resource group Matrials exists and if not create it
		if (std::find(resourceGroups.begin(), resourceGroups.end(), "Materials") == resourceGroups.end())
		{
			Ogre::ResourceGroupManager::getSingletonPtr()->createResourceGroup("Materials");
		}

		// Create material 
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().
			create(name + "PDMaterial", "Materials");

		material->setReceiveShadows(false);
		material->getTechnique(0)->setLightingEnabled(true);
		material->getTechnique(0)->getPass(0)->setSelfIllumination(color.x, color.y, color.z);
		material->getTechnique(0)->getPass(0)->setDiffuse(color.x, color.y, color.z, 1.f);
		material->getTechnique(0)->getPass(0)->setAmbient(color.x, color.y, color.z);
		material->getTechnique(0)->getPass(0)->setShininess(110);
		material->getTechnique(0)->getPass(0)->setDepthBias(1.0f);

		_renderedLines = _ogreMgr->createManualObject();
		_renderedLines->setCastShadows(false);
		_renderedLines->setDynamic(true);
		_renderedLines->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAIN);
	}

	PathingDrawer::~PathingDrawer()
	{
		ClearRender();
		_ogreMgr->destroyManualObject(_renderedLines);
	}


	void PathingDrawer::DrawLine(const PFVector2& startPoint, const PFVector2& endPoint)
	{
		float startY = GameWorld::GlobalWorld->GetMap()->GetTerrainHeight(startPoint.x, startPoint.y);
		float endY = GameWorld::GlobalWorld->GetMap()->GetTerrainHeight(endPoint.x, endPoint.y);
		_pointList.push_back(Vector3(startPoint.x, startY+4.0f, startPoint.y));
		_pointList.push_back(Vector3(endPoint.x, endY+4.0f, endPoint.y));
	}

	void PathingDrawer::DrawMessage(const PFVector2& position, const PFVector2& size, const char* text)
	{

	}

	void PathingDrawer::ClearRender()
	{
		if (_linesDrawn)
		{
			const unsigned int size = _pointList.size();
			_linesDrawn = false;
			_pointList.clear();
			_pointList.reserve(size);

			_renderedLines->clear();
		}
	}

	void PathingDrawer::Draw()
	{
		if (_linesDrawn || 
			_pointList.empty())
		{
			return;
		}
		else
		{
			_linesDrawn = true;
		}

		_renderedLines->estimateVertexCount(_pointList.size());
		_renderedLines->begin(_name + "PDMaterial", Ogre::RenderOperation::OT_LINE_LIST);

		for (auto vertexIt = _pointList.begin(), end = _pointList.end();
			vertexIt != end; ++vertexIt)
		{
			_renderedLines->position(*vertexIt);
			++vertexIt;
			_renderedLines->position(*vertexIt);
		}

		_renderedLines->end();
	}


	void PathingDrawer::AttachToScene(Ogre::SceneNode* rootNode)
	{
		DetachFromScene();
		_debugNode = rootNode->createChildSceneNode(_name + "PDNode");
		_debugNode->attachObject(_renderedLines);
	}

	void PathingDrawer::DetachFromScene()
	{
		if(_debugNode != 0)
		{
			_debugNode->detachAllObjects();
			Ogre::SceneNode* parentNode = _debugNode->getParentSceneNode();
			parentNode->removeAndDestroyChild(_name + "PDNode");
			_debugNode = 0;
		}
	}

}