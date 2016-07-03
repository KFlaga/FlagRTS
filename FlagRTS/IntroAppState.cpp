#include "IntroAppState.h"
#include "AppManager.h"
#include "Constants.h"
#include "LoadingScreen.h"
#include <Memory.>
#include <Managers.h>
#include <OIS\OISKeyboard.h>
#include <Ogre\Ogre.h>
#include "MainMenuState.h"
#include <ConfigurationManager.h>


namespace FlagRTS
{
	IntroAppState::IntroAppState() :
		_ogre(RenderSystemManager::Instance()->GetOgreRoot()),
		_mainWindow(RenderSystemManager::Instance()->GetMainWindow()),
		_loadingScreen(0)
	{

	}

	IntroAppState::~IntroAppState()
	{
		xDeleteSafe(_loadingScreen);
	}

	ErrorCode IntroAppState::Load()
	{
		ADD_LOG("Begin Loading IntroAppState");
		// Create ogre objects and loading screen
		_sceneManager = _ogre->createSceneManager(Ogre::ST_GENERIC, "IntroStateSceneManager");
		
		_camera = _sceneManager->createCamera("IntroCamera");
		_camera->setNearClipDistance(1);
		Ogre::Viewport* vp = _mainWindow->addViewport(_camera);
		vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
		_camera->setAspectRatio(vp->getWidth() / vp->getHeight());

		GUIManager::Instance()->InitOgre(_sceneManager, _mainWindow);
		
		_loadingScreen = xNew0(LoadingScreen);

		ADD_LOG("End Loading IntroAppState");
		return ErrorCodes::Success;
	}

	ErrorCode IntroAppState::Unload()
	{
		if(_loadingScreen)
			_loadingScreen->Unload();
		GUIManager::Instance()->ChangeSceneManager(nullptr);
		_sceneManager->destroyAllCameras(); // ogre should handle it
		_ogre->destroySceneManager(_sceneManager);

		return ErrorCodes::Success;
	}

	ErrorCode IntroAppState::Start()
	{
		ADD_LOG("Begin Start: IntroAppState");
		_loadingScreen->Show();

		AppManager::Instance()->SheduleTask(xNew1(TaskInitResources,(this)));
		AppManager::Instance()->SheduleTask(xNew1(TaskLoadLocale,(this)));
		AppManager::Instance()->SheduleTask(xNew1(TaskLoadSounds,(this)));
		AppManager::Instance()->SheduleTask(xNew1(TaskPushMainMenu,(this)));
		
		ADD_LOG("End Start: IntroAppState");
		
		return ErrorCodes::Success;
	}

	ErrorCode IntroAppState::End()
	{
		Unload();
		return ErrorCodes::Success;
	}

	void IntroAppState::InitResources()
	{
		_loadingScreen->SetLoadingStatus("Initializing resources");
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		
		RefPtr<Configuration> mainConf = ConfigurationManager::Instance()->GetConfiguration("Main");
		CursorManager::Instance()->InitCursors(
			mainConf->FindOptionValue("Resources","Directories") + "Cursors\\");
	}

	void IntroAppState::LoadLocale()
	{
		_loadingScreen->SetLoadingStatus("Loading localization");
		LocalizationManager::Instance()->LoadLanguages();
	}

	void IntroAppState::LoadSounds()
	{
		_loadingScreen->SetLoadingStatus("Loading sounds");
		Media::AudioManager::Instance()->InitSoundManager();

		// Find base sounds directory
		RefPtr<Configuration> mainConf = ConfigurationManager::Instance()->GetConfiguration("Main");
		string resDir = mainConf->FindOptionValue("Resources", "Directories");
		// Load base sounds
		Media::AudioManager::Instance()->AddSoundDoc(resDir + "Sounds\\");
		Media::AudioManager::Instance()->LoadSoundPack("Gui");
	}

	void IntroAppState::PushMainMenu()
	{
		_loadingScreen->SetLoadingStatus("Building main menu");
		AppManager::Instance()->PushState("MainMenu");
	}

	ErrorCode IntroAppState::Update(float ms)
	{
		// update input and gui
		InputManager::Instance()->Update(ms);
		// If esc is pressed - end app
		if(InputManager::Instance()->CheckKeyIsDown(OIS::KC_ESCAPE))
		{
			return ErrorCodes::Exit;
		}

		GUIManager::Instance()->Update(ms);

		// get info on current  loading state and update state text
		MainMenuState* mmstate = (MainMenuState*)(AppManager::Instance()->GetState("MainMenu"));
		if(mmstate != 0)
		{
			string s = mmstate->GetLoadingStatus();
			_loadingScreen->SetLoadingStatus(s);
		}
	//	_loadingScreen->SetLoadingStatus(std::to_string(ms));

		return ErrorCodes::Success;
	}
}