#include "MapPreview.h"
#include <OgreSceneManager.h>
#include <OgreTextureManager.h>
#include <OgreMaterialManager.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreManualObject.h>
#include <OgreBillboard.h>
#include <OgreBillboardSet.h>
#include <RenderSystemManager.h>
#include <MyGUI\MyGUI_ImageBox.h>
#include <InputManager.h>
#include <MyGUI\MyGUI_OgreTexture.h>

namespace FlagRTS
{
	MapPreview::MapPreview(MyGUI::ImageBox* imageBox) :
		_ogreMgr(RenderSystemManager::Instance()->GetOgreRoot()->
		createSceneManager(Ogre::ST_GENERIC)),
		_imageWidget(imageBox)
	{
		
		_ogreMgr->setAmbientLight(Ogre::ColourValue(0.9f, 0.9f, 0.9f));
		Ogre::Vector3 dir(-1.f, -1.f, -1.f);
		dir.normalise();
		Ogre::Light* light = _ogreMgr->createLight(_ogreMgr->getName()+"Light");
		light->setType(Ogre::Light::LT_DIRECTIONAL);
		light->setDirection(dir);
		light->setDiffuseColour(0.8f, 0.8f, 0.8f);
		light->setSpecularColour(0.8f, 0.8f, 0.8f);

		_finalTexture = Ogre::TextureManager::getSingleton().createManual(
			"MapPreviewTex", 
			"MyGUI", 
			Ogre::TEX_TYPE_2D, 
			_imageWidget->getWidth(), _imageWidget->getHeight(), 0,
			Ogre::PF_R8G8B8, 
			Ogre::TU_RENDERTARGET);

		MyGUI::OgreTexture* myguiTexture = new MyGUI::OgreTexture("MapPreviewMyGUITex", "MyGUI");
		myguiTexture->setOgreTexture(_finalTexture);
		_imageWidget->setImageTexture(_finalTexture->getName());

		_worldSize = Vector2(_imageWidget->getWidth(), _imageWidget->getHeight());
		float minSize = std::min(_worldSize.x, _worldSize.y);
		_startPositionSize = minSize * 0.1f;
		CreateRenderCamera();
		CreateTerrainMaterial();
		CreateTerrainPlane();
		CreateBillboards();

		_imageWidget->setVisible(false);
		_imageWidget->eventMouseButtonPressed += MyGUI::newDelegate(this,
			&MapPreview::MousePressed);
	}

	MapPreview::~MapPreview()
	{

	}

	void MapPreview::Update(float ms)
	{
		_startPositionsBillboardSet->_updateBounds();
		_highlightsBillboardSet->_updateBounds();
	}

	void MapPreview::MoveStartingPosition(int number, const Vector2& pos)
	{
		_startPositions[number]->setPosition(pos.x, 5.f, pos.y);
		_highlights[number]->setPosition(pos.x, _highlights[number]->getPosition().y, pos.y);
	}

	void MapPreview::SetStartingPositionColor(int number, const Vector3& color)
	{
		_startPositions[number]->setColour(Ogre::ColourValue(color.x, color.y, color.z));
	}

	void MapPreview::DisableStartingPosition(int number)
	{
		_startPositions[number]->setPosition(-1000.f, -1000.f, -1000.f);
		_highlights[number]->setPosition(-1000.f, -1000.f, -1000.f);
	}

	void MapPreview::EnableStartPositionHighlight(int number)
	{
		_highlights[number]->setPosition(
			_startPositions[number]->getPosition().x, 
			2.f, 
			_startPositions[number]->getPosition().z);
	}

	void MapPreview::DisableStartPositionHighlight(int number)
	{
		_highlights[number]->setPosition(
			_startPositions[number]->getPosition().x, 
			-1000.f, 
			_startPositions[number]->getPosition().z);
	}

	void MapPreview::Show()
	{
		// Create render viewport for final texture
		Ogre::RenderTexture* renderTexture = _finalTexture->getBuffer()->getRenderTarget();
		renderTexture->addViewport(_renderCamera);
		renderTexture->getViewport(0)->setClearEveryFrame(true);
		renderTexture->getViewport(0)->setBackgroundColour(Ogre::ColourValue::White);
		renderTexture->getViewport(0)->setOverlaysEnabled(false);
		renderTexture->getViewport(0)->setShadowsEnabled(false);
		renderTexture->getViewport(0)->setSkiesEnabled(false);

		_imageWidget->setVisible(true);
	}

	void MapPreview::Hide()
	{	
		_imageWidget->setVisible(false);
		Ogre::RenderTexture* renderTexture = _finalTexture->getBuffer()->getRenderTarget();
		renderTexture->removeAllViewports();
	}

	void MapPreview::SetNewMap(
		const Vector2& mapSize,
		const string& mapImagePath,
		const string& mapImageName,
		int startingPositionsCount)
	{
		Hide();

		_worldSize = mapSize;
		LoadTerrainTexture(mapImagePath, mapImageName);

		// Update camera/background size/position
		_renderCamera->setFrustumExtents(-_worldSize.x*0.5f, 
			_worldSize.x*0.5f, _worldSize.y*0.5f, -_worldSize.y*0.5f);
		_cameraNode->setPosition(Vector3(_worldSize.x*0.5f,300.f,_worldSize.y*0.5f));

		_bgNode->setPosition(Vector3(_worldSize.x*0.5f,0.f,_worldSize.y*0.5f));
		_bgNode->setScale(Vector3(_worldSize.x/(float)_imageWidget->getWidth(), 
			1.f, _worldSize.y/(float)_imageWidget->getHeight()));

		float minSize = std::min(_worldSize.x, _worldSize.y);
		_startPositionsBillboardSet->setBounds(Ogre::AxisAlignedBox(
			Vector3(-1000.f,-1000.f,-1000.f), 
			Vector3(_worldSize.x, 100.f, _worldSize.y)), 
			1000.f + std::max(_worldSize.x, _worldSize.y));

		_startingPositionsCount = startingPositionsCount;
		_startPositionSize = minSize * 0.1f;
		// Move all starting positions to invisible part of map
		for( unsigned int i = 0; i < 8; ++i)
		{
			_startPositions[i]->setPosition(-1000.f,-1000.f,-1000.f);
			_startPositions[i]->setDimensions(_startPositionSize.x, _startPositionSize.y);
			_highlights[i]->setPosition(-1000.f,-1000.f,-1000.f);
			_highlights[i]->setDimensions(_startPositionSize.x, _startPositionSize.y);
		}

		Show();
	}

	void MapPreview::CreateRenderCamera()
	{
		// Camera points directly top->down, placed on center of internal world
		_renderCamera = _ogreMgr->createCamera("MapPreviewCamera");
		_renderCamera->setDirection(Vector3::NEGATIVE_UNIT_Z);
		_renderCamera->setFarClipDistance(500.f);
		_renderCamera->setNearClipDistance(100.f);
		_renderCamera->setProjectionType(Ogre::ProjectionType::PT_ORTHOGRAPHIC);
		_renderCamera->setFrustumExtents(-_worldSize.x*0.5f, 
			_worldSize.x*0.5f, _worldSize.y*0.5f, -_worldSize.y*0.5f);

		_cameraNode = _ogreMgr->getRootSceneNode()->createChildSceneNode(
			"MapPreviewCameraNode", Vector3(_worldSize.x*0.5f,300.f,_worldSize.y*0.5f), 
			Quaternion(Radian(Degree(-90.f)), Vector3(1.f,0.f,0.f)));
		_cameraNode->attachObject(_renderCamera);
	}

	void MapPreview::CreateTerrainPlane()
	{
		_bgNode = _ogreMgr->getRootSceneNode()->createChildSceneNode(
			"MapPreviewBgNode", Vector3(_worldSize.x*0.5f,0.f,_worldSize.y*0.5f));

		_bgPlane = _ogreMgr->createManualObject();
		_bgPlane->estimateVertexCount(4);
		_bgPlane->estimateIndexCount(6);

		_bgPlane->begin("MapPreviewTerrainMat");
		_bgPlane->position(-_worldSize.x*0.5f, 0.f,-_worldSize.y*0.5f);
		_bgPlane->textureCoord(0.f,0.f);
		_bgPlane->position(-_worldSize.x*0.5f, 0.f,_worldSize.y*0.5f);
		_bgPlane->textureCoord(0.f,1.f);
		_bgPlane->position(_worldSize.x*0.5f, 0.f,_worldSize.y*0.5f);
		_bgPlane->textureCoord(1.f,1.f);
		_bgPlane->position(_worldSize.x*0.5f, 0.f,-_worldSize.y*0.5f);
		_bgPlane->textureCoord(1.f,0.f);
		_bgPlane->quad(0,1,2,3);
		_bgPlane->end();

		_bgNode->attachObject(_bgPlane);
	}

	void MapPreview::CreateBillboards()
	{
		_billNode = _ogreMgr->getRootSceneNode()->createChildSceneNode(
			"MapPreviewBillboardNode");

		_startPositions.resize(8);
		_highlights.resize(8);
		_startPositionsBillboardSet = _ogreMgr->createBillboardSet(
			"MapPreviewStartBillboard", _startPositions.size() + 1);
		_highlightsBillboardSet = _ogreMgr->createBillboardSet(
			"MapPreviewHighlightsBillboard", _highlights.size() + 1);

		_startPositionsBillboardSet->setDefaultDimensions(_startPositionSize.x, _startPositionSize.y);
		_startPositionsBillboardSet->setMaterialName("MapPreviewStartBillboardMat", "Maps");
		_highlightsBillboardSet->setDefaultDimensions(_startPositionSize.x, _startPositionSize.y);
		_highlightsBillboardSet->setMaterialName("MapPreviewHighlightBillboardMat", "Maps");

		for( unsigned int i = 0; i < 8; ++i)
		{
			_startPositions[i] = _startPositionsBillboardSet->createBillboard(-1000.f,-1000.f,-1000.f);
			_highlights[i] = _highlightsBillboardSet->createBillboard(-1000.f,-1000.f,-1000.f);
		}

		_billNode->attachObject(_startPositionsBillboardSet);
		_billNode->attachObject(_highlightsBillboardSet);
	}

	void MapPreview::CreateTerrainMaterial()
	{
		auto material = Ogre::MaterialManager::getSingleton().
			create("MapPreviewTerrainMat", "Maps");

		material->setReceiveShadows(false);

		Ogre::TextureUnitState *texState = material->getTechnique(0)->getPass(0)->
			createTextureUnitState("MapPreviewDefaultMap.png");

		material->load();
	}

	void MapPreview::LoadTerrainTexture(const string& terrainImagePath, 
		const string& terrainImageName)
	{
		auto texture = Ogre::TextureManager::getSingleton().
			getByName(terrainImagePath, "Maps");

		if( texture.isNull() )
		{

			Ogre::Image img;
			std::ifstream imgStream(terrainImagePath.c_str(), std::ios::binary | std::ios::in);
			if (imgStream.is_open())
			{
				Ogre::String ext;
				Ogre::String::size_type extIdx = terrainImagePath.find_last_of('.');

				ext = terrainImagePath.substr(extIdx+1);
				Ogre::DataStreamPtr dataStream(
					new Ogre::FileStreamDataStream(terrainImagePath, &imgStream, false));

				img.load(dataStream, ext);

				imgStream.close();
			}
			else
			{
				_ASSERT("Bad MapPreview image path");
			}

			texture = Ogre::TextureManager::getSingleton().createManual(
				terrainImagePath, "Maps", 
				Ogre::TEX_TYPE_2D, 
				img.getWidth(), img.getHeight(), 
				0, img.getFormat(),
				Ogre::TU_DEFAULT);

			texture->loadImage(img);
		}

		auto material = Ogre::MaterialManager::getSingleton().
			getByName("MapPreviewTerrainMat", "Maps");

		material->getTechnique(0)->getPass(0)->
			removeAllTextureUnitStates();
		Ogre::TextureUnitState *texState = material->getTechnique(0)->getPass(0)->
			createTextureUnitState(texture->getName());

		if(material->isLoaded())
			material->unload();
		material->load();

		_bgPlane->setMaterialName(0, material->getName(), material->getGroup());
	}

	void MapPreview::MousePressed(MyGUI::Widget*, int x, int y, MyGUI::MouseButton button)
	{
		// Get mouse button / position of click
		auto input = InputManager::Instance()->GetInputState();
		if( button != MyGUI::MouseButton::Left && button != MyGUI::MouseButton::Right )
			return; // Middle or any other button was clicked -> not intrested

		// Get mouse pos in world coords
		const auto& mousePos = input->GetMouseScreenAbsPos();
		auto widgetCoords = _imageWidget->getAbsoluteCoord();
		Vector2 worldCoords = Vector2( 
			(mousePos.x - widgetCoords.left) * (_worldSize.x / widgetCoords.width),
			(mousePos.y - widgetCoords.top) * (_worldSize.y / widgetCoords.height));

		// Check if mouse hovers over any of starting positions
		for(unsigned int i = 0; i < _startingPositionsCount; ++i)
		{
			Vector2 billPos(_startPositions[i]->getPosition().x, 
				_startPositions[i]->getPosition().z);

			if( !(billPos.x - _startPositionSize.x*0.5f > worldCoords.x ||
				billPos.x + _startPositionSize.x*0.5f < worldCoords.x ||
				billPos.y - _startPositionSize.y*0.5f > worldCoords.y ||
				billPos.y + _startPositionSize.y*0.5f < worldCoords.y) )
			{
				// starting position was hit
				if( button == MyGUI::MouseButton::Right )
				{
					_startPositionRightClicked.Fire(i);
				}
				else
				{
					_startPositionLeftClicked.Fire(i);
				}
				break;
			}
		}
	}

	void MapPreview::ResetMap()
	{
		auto material = Ogre::MaterialManager::getSingleton().
			getByName("MapPreviewTerrainMat", "Maps");

		material->getTechnique(0)->getPass(0)->
			removeAllTextureUnitStates();
		Ogre::TextureUnitState *texState = material->getTechnique(0)->getPass(0)->
			createTextureUnitState("MapPreviewDefaultMap.png");

		for( unsigned int i = 0; i < 8; ++i)
		{
			_startPositions[i]->setPosition(-1000.f,-1000.f,-1000.f);
			_highlights[i]->setPosition(-1000.f,-1000.f,-1000.f);
		}
	}
}