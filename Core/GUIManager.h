#pragma once
#pragma once

#include "GenericManager.h"
#include <map>
#include "GUI.h"

#include <MyGui\MyGUI.h>
#include <MyGUI\MyGUI_OgrePlatform.h>

namespace Ogre
{
	class SceneManager;
}

namespace FlagRTS
{
	// GUIManager stores all registered guis, allowing easy switch between them
	// When active gui is changed, it unloads all layouts of old one and loads all of new one
	// Contains MyGUI::GUI and MyGUI::OgrePlatform and intializes them
	// 
	class GUIManager : public GenericManager<GUIManager>
	{
	private:
		MyGUI::Gui* _mygui;
		MyGUI::OgrePlatform* _ogre;

		typedef std::map<std::string, RefPtr<GUI>, string_less> GuiMap;
		GuiMap _guis;

		RefPtr<GUI> _currentGui;

	protected:
		GUIManager();
		friend GenericManager<GUIManager>;

	public:
		ErrorCode InitOgre(Ogre::SceneManager* sceneMgr, Ogre::RenderWindow* window);
		
		ErrorCode RegisterGUI(const std::string& guiName, RefPtr<GUI> gui);
		ErrorCode UnregisterGUI(const std::string& guiName);
		RefPtr<GUI> FindGUI(const std::string& guiName);

		// Updates gui input (injects key/mouse events), updates current GUI
		 void Update(float ms);

		ErrorCode SetCurrentGUI(const std::string& guiName);

		void ChangeSceneManager(Ogre::SceneManager* sceneMgr)
		{
			(*_ogre).getRenderManagerPtr()->setSceneManager(sceneMgr);
		}

		void ChangeViewport(uint16 viewport)
		{
			(*_ogre).getRenderManagerPtr()->setActiveViewport(viewport);
		}

		void ChangeRenderWindow(Ogre::RenderWindow* window)
		{
			(*_ogre).getRenderManagerPtr()->setRenderWindow(window);
		}
		
		~GUIManager();
	};
}