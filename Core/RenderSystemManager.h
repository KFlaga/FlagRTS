#pragma once

#include "GenericManager.h"
#include "TypeDefs.h"
#include "Pointers.h"
#include <OGRE\Ogre.h>
#include "Configuration.h"
#include "Event.h"
#include "IntVector2.h"

#define VIEWPORT_MENU 10
#define VIEWPORT_GAME 1
#define VIEWPORT_GUI 3
#define VIEWPORT_MAP 4

namespace FlagRTS
{
	// Stores ogre root and window objects
	// Allows changes in reder system and render/window options
	// Handles window events
	class RenderSystemManager : public GenericManager<RenderSystemManager> , public Ogre::WindowEventListener
	{
	private:
		Ogre::Root* _ogre;
		Ogre::RenderWindow* _mainWindow;
		size_t _hWnd;
		uint16 _winWidth, _winHeight;
		bool _rendering;
		bool _closePending;
		bool _isFullscreen;

		Event<std::pair<int,int>> _onResolutionChanged;
		Event<std::pair<int,int>> _onWindowResized;
		Event<bool> _onFullscreenChanged;

	protected:
		RenderSystemManager();
		friend GenericManager<RenderSystemManager>;

	public:
		~RenderSystemManager();

		bool Render(); // Renders one frame
		bool IsRendering(); // Checks if ogre is rendering -> during render objects shouldn't be added to scene
		
		// Assumes that general config is loaded
		// Initializes OgreRoot, loads render system and plugins, creates window
		ErrorCode InitOgre();

		// Return handle to main window
		size_t GetHwnd() const { return _hWnd; }

		// Returns main window
		Ogre::RenderWindow* const GetMainWindow() { return _mainWindow; }

		// Returns ogre root
		Ogre::Root* GetOgreRoot() const { return _ogre; }

		Ogre::RenderSystemList GetAvailableRenderSystems()
		{
			return _ogre->getAvailableRenderers();
		}

		Ogre::StringVector GetAvailableVideoModes()
		{
			auto opts = _ogre->getRenderSystem()->getConfigOptions();
			auto modes = opts.find("Video Mode");
			return modes->second.possibleValues;
		}

		// Returns trfu if window is in fullscreen mode
		bool IsFullscreen() const { return _mainWindow->isFullScreen(); }
		// Changes window into fullscreen mode ( if changed together with resoultion,
		// change resolution (video mode) first )
		void SetFullscreen(bool fullscreen);

		// Returns true of vsync is enabled
		bool IsVSync() const
		{
			return _mainWindow->isVSyncEnabled();
		}
		// Enable/disable vsync
		void SetVSync(bool enabled)
		{
			_mainWindow->setVSyncEnabled(enabled);
		}

		// Retruns string with info on window resolution
		string GetVideoMode() const
		{
			return std::to_string(_winWidth) + " x " + std::to_string(_winHeight) + " @ 32-bit colour";
		}

		IntVector2 GetWindowSize() const
		{
			return IntVector2(_winWidth, _winHeight);
		}

		// Sets window resolution, given string in form : "W x H @ XX-bit colour"
		void SetVideoMode(const string& mode);

		void SetVideoMode(const int width, const int height);

		Event<std::pair<int,int>>& ResolutionChanged() { return _onResolutionChanged; }
		Event<std::pair<int,int>>& WindowResized() { return _onWindowResized; }
		Event<bool>& FullscreenChanged() { return _onFullscreenChanged; }

		// WindowEventListener implementation:

		// Called when 'X' button is pressed and windowC
		void windowClosed(Ogre::RenderWindow* rw);
		// Called when 'X' button is pressed. If true is returned, closes window
		bool windowClosing(Ogre::RenderWindow* rw);
		// Called when window has lost/gained focus
		void windowFocusChange(Ogre::RenderWindow* rw);
		// Called when window changes position
		void windowMoved(Ogre::RenderWindow* rw);
		// Called when window is resized
		void windowResized(Ogre::RenderWindow* rw);
	
	private:
		void LoadOgrePlugins(const RefPtr<Configuration>& config);
		ErrorCode InitRenderSystem(const std::string& system);
		ErrorCode LoadRenderSystemOptions(const RefPtr<Configuration>& config);
	};
}