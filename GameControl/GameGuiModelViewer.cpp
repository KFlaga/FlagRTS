#include "GameGuiModelViewer.h"
#include <PhysicalObject.h>
#include <GameWorld.h>
#include <IGameObjectPool.h>
#include <OgreSceneManager.h>
#include <RenderSystemManager.h>
#include <MyGUI\MyGUI_OgreTexture.h>
#include <MyGUI\MyGUI_Gui.h>
#include <MyGUI\MyGUI_ImageBox.h>

namespace FlagRTS
{
	using namespace GameGui;

	ModelViewer::ModelViewer(int texWidth, int texHeight, MyGUI::Widget* parent) :
		_sceneMgr(RenderSystemManager::Instance()->GetOgreRoot()->
		createSceneManager(Ogre::ST_GENERIC)),
		_model(_sceneMgr, _sceneMgr->getName(), texWidth, texHeight, Vector3::ZERO),
		_viewedObject(0),
		_mainPanel(0)
	{
		if(parent == 0)
		{
			_mainPanel = MyGUI::Gui::getInstancePtr()->createWidget<MyGUI::ImageBox>("ImageBox",
				0, 0, texWidth, texHeight, MyGUI::Align::Default, _sceneMgr->getName()+"Panel");
		}
		else
		{
			_mainPanel = parent->createWidget<MyGUI::ImageBox>("ImageBox",
				0, 0, texWidth, texHeight, MyGUI::Align::Default, _sceneMgr->getName()+"Panel");
		}

		_sceneMgr->setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.3f));
		Ogre::Vector3 dir(-1.f, -1.f, -1.f);
		dir.normalise();
		Ogre::Light* light = _sceneMgr->createLight(_sceneMgr->getName()+"Light");
		light->setType(Ogre::Light::LT_DIRECTIONAL);
		light->setDirection(dir);
		light->setDiffuseColour(0.8f, 0.8f, 0.8f);
		light->setSpecularColour(0.8f, 0.8f, 0.8f);

		_model.SetBackgroundColor(Ogre::ColourValue(0.1f,0.1f,0.1f,1.f));
		_model.SetRenderObject(0);

		auto ogreTexture = _model.GetRenderTexture();
		MyGUI::OgreTexture* myguiTexture = new MyGUI::OgreTexture(_sceneMgr->getName()+"Tex", "MyGUI");
		myguiTexture->setOgreTexture(ogreTexture);
		_mainPanel->setImageTexture(_model.GetRenderTexture()->getName());
	}

	ModelViewer::~ModelViewer()
	{
		// TODO Unload all from map
	}

	void ModelViewer::Update(float ms)
	{

	}

	void ModelViewer::SetViewedObject(PhysicalObjectDefinition* objectDef)
	{
		if(objectDef == 0)
		{
			_model.SetRenderObject(0);
			_viewedObject = 0;
			return;
		}

		auto objectIt = _displayObjects.find(objectDef);
		if(objectIt != _displayObjects.end())
		{
			// Reuse object
			_viewedObject = objectIt->second;
		}
		else
		{
			// Create object instance for this object kind
			_viewedObject = static_cast<PhysicalObject*>(
				GameInterfaces::GetGameObjectPool()->Create(objectDef, NEUTRAL_PLAYERNUM));

			_viewedObject->LoadResources(_sceneMgr);
			_viewedObject->ChangeState(SceneObjectStates::NotSpawned);
			_displayObjects[objectDef] = _viewedObject;
		}
		// Set object for ModelTexture - it should handle rest
		_model.SetRenderObject(_viewedObject);
	}

	void ModelViewer::SetVisible(bool value)
	{
		_mainPanel->setVisible(value);
	}

	bool ModelViewer::IsVisible() const
	{
		return _mainPanel->getVisible();
	}

	void ModelViewer::Resize( float ratioHorizontal, float ratioVertical )
	{
		const MyGUI::IntCoord& oldCoord = GetPanel()->getCoord();
		GetPanel()->setCoord(
			(int)((float)oldCoord.left * ratioHorizontal), (int)((float)oldCoord.top * ratioVertical), 
			(int)((float)oldCoord.width * ratioHorizontal), (int)((float)oldCoord.height * ratioVertical));
	}

	void ModelViewer::SetObjectPosition(const Vector3& pos)
	{
		_model.SetPosition(pos);
	}

	void ModelViewer::SetObjectOrientation(const Quaternion& q)
	{
		_model.SetOrientation(q);
	}
}