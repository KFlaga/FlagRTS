#pragma once

#include <ErrorCodes.h>
#include <Pointers.h>
#include <GUIManager.h>
#include <TypeId.h>
#include <Event.h>

namespace FlagRTS
{
	// Interface for main menu panels ( main menu screen, options screen etc ), which
	// are changed from main menu screen
	class IMenuScreen
	{
	protected:
		RefPtr<GUI> _gui;
		string _name;
		Event<string> _changeScreenRequested;

	public:
		IMenuScreen(RefPtr<GUI> gui, const string& name) : 
			_gui(gui), _name(name)
		{ 
			
		}

		virtual ~IMenuScreen() { }

		virtual ErrorCode Update(float ms)
		{
			return ErrorCodes::Success;
		}

		virtual void Show()
		{
			GUIManager::Instance()->SetCurrentGUI(_gui->GetName());
		}

		virtual void Hide()
		{
			_gui->HideAllLayouts();
		}

		virtual void Unload()
		{
			_gui->UnloadAllLayouts();
		}

		string GetName() const
		{
			return _name;
		}

		virtual void Resize(int width, int height) { }

		Event<string>& GetChangeScreenRequestedEvent() { return _changeScreenRequested; }
	};
}