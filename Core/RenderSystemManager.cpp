#include "RenderSystemManager.h"
#include "ErrorManager.h"
#include "LogManager.h"
#include "ConfigurationManager.h"
#include "InputManager.h"
#include <sstream>


namespace FlagRTS
{
	RenderSystemManager* RenderSystemManager::_instance = 0;

	RenderSystemManager::RenderSystemManager() :
		_ogre(new Ogre::Root("", "")),
		_closePending(false)
	{

	}

	bool RenderSystemManager::Render()
	{
		if(_closePending)
			return false;

		if(::GetForegroundWindow() == (HWND)_hWnd)
		{
			_rendering = true;
			_ogre->renderOneFrame();
			_rendering = false;
		}

		return true;
	}

	RenderSystemManager::~RenderSystemManager()
	{
		delete _ogre;
	}

	ErrorCode RenderSystemManager::InitOgre()
	{
		ADD_LOG("Initializing OGRE");

		RefPtr<Configuration> settingsConfig = ConfigurationManager::Instance()->GetConfiguration("Settings");
		RefPtr<Configuration> mainConfig = ConfigurationManager::Instance()->GetConfiguration("Main");
		ErrorCode ec;

		LoadOgrePlugins(mainConfig);
		std::string renderSystem = (*settingsConfig).FindOptionValue("RenderSystem", "Video.RenderSystem");
		ec = InitRenderSystem(renderSystem);

		if (ec != ErrorCodes::Success) 
		{
			ADD_LOG(string("Specified render system (") + renderSystem + string(") not found"));
			return ec;
		}
		ADD_LOG(string("Render system : ") + renderSystem + string(" found"));

		ec = LoadRenderSystemOptions(settingsConfig);
		if( ec != ErrorCodes::Success )
		{
			ADD_LOG(string("Coundn't load render system options"));
			return ec;
		}

		_mainWindow = _ogre->initialise(true, "FlagRTS");
		_mainWindow->getCustomAttribute("WINDOW",&_hWnd);
		Ogre::WindowEventUtilities::addWindowEventListener(_mainWindow, this);

		ADD_LOG("RenderWindow created");

		return ErrorCodes::Success;
	}

	void  RenderSystemManager::LoadOgrePlugins(const RefPtr<Configuration>& config)
	{
		SectionIterator pluginsIt;
		SectionIterator sectionEnd;
		if( config->Begin("Plugins.Ogre", pluginsIt) == ErrorCodes::Success)
		{
			config->End("Plugins.Ogre", sectionEnd);
			for(;pluginsIt != sectionEnd; pluginsIt++)
			{
				_ogre->loadPlugin((*pluginsIt).Value->Value);
			}
		}
	}

	ErrorCode  RenderSystemManager::InitRenderSystem(const std::string& system)
	{
		Ogre::RenderSystemList renderSystems;
		Ogre::RenderSystemList::iterator r_it;

		renderSystems = _ogre->getAvailableRenderers();

		// check through the list of available renderers, looking for the one that contains
		// renderer from configuration file
		for (r_it= renderSystems.begin(); r_it != renderSystems.end(); r_it++) 
		{
			Ogre::RenderSystem *tmp = *r_it;
			std::string rName(tmp->getName());
			if ((int) rName.find(system) >= 0) 
			{
				_ogre->setRenderSystem(*r_it);
				return ErrorCodes::Success;
			}
		}
		return ErrorCodes::RenderSystemNotFound;
	}

	ErrorCode  RenderSystemManager::LoadRenderSystemOptions(const RefPtr<Configuration>& config)
	{
		SectionIterator renderOpts;
		if( config->Begin("Video.Settings", renderOpts) != ErrorCodes::Success)
			return ErrorCodes::RenderSystemFailedToLoadOptions;

		SectionIterator endSection;
		config->End("Video.Settings", endSection);
		for(;renderOpts != endSection; renderOpts++)
		{
			_ogre->getRenderSystem()->setConfigOption(renderOpts->Value->Name, (*renderOpts).Value->Value);
		}

		return ErrorCodes::Success;
	}

	void RenderSystemManager::SetVideoMode(const string& mode)
	{
		// "W x H @ XX-bit colour"
		int height, width;
		char x;
		std::stringstream sstream;
		sstream << mode;
		sstream >> width >> x >> height;
		SetVideoMode(width, height);
	}

	void RenderSystemManager::SetVideoMode(const int width, const int height)
	{
		_winWidth = width;
		_winHeight = height;
		_mainWindow->resize(_winWidth, _winHeight);

		if( _isFullscreen == false )
		{
			int screenw = GetSystemMetrics(SM_CXSCREEN);
			int screenh = GetSystemMetrics(SM_CYSCREEN);
			int left = (screenw - _winWidth) / 2;
			int top = (screenh - _winHeight) / 2;

			_mainWindow->reposition(left, top);
		}
	}

	void RenderSystemManager::SetFullscreen(bool fullscreen) 
	{ 
		_isFullscreen = fullscreen;
		_mainWindow->setFullscreen(fullscreen, _winWidth, _winHeight);
		if( _isFullscreen == false )
		{
			int screenw = GetSystemMetrics(SM_CXSCREEN);
			int screenh = GetSystemMetrics(SM_CYSCREEN);
			int left = (screenw - _winWidth) / 2;
			int top = (screenh - _winHeight) / 2;

			_mainWindow->reposition(left, top);
		}
	}

	void RenderSystemManager::windowClosed(Ogre::RenderWindow* rw)
	{
		_closePending = true;
	}

	bool RenderSystemManager::windowClosing(Ogre::RenderWindow* rw)
	{
		return true;
	}

	void RenderSystemManager::windowFocusChange(Ogre::RenderWindow* rw)
	{

	}

	void RenderSystemManager::windowMoved(Ogre::RenderWindow* rw)
	{

	}

	void RenderSystemManager::windowResized(Ogre::RenderWindow* rw)
	{
		_onWindowResized.Fire(std::make_pair(rw->getWidth(), rw->getHeight()));
	}
}