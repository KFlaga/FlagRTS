#pragma once

#include "Pointers.h"
#include "TypeDefs.h"
#include "Array.h"

namespace MyGUI
{
	class Widget;
}

namespace FlagRTS
{
	namespace GameGui
	{
		class GuiElement;
	}

	// Simple container for mygui widgets
	// Except MyGUI widgets stores parent widget for easy parent-child hierarchy
	//
	class Layout
	{
	public:
		MyGUI::Widget* Parent;
		std::vector<MyGUI::Widget*> Widgets;
		string Name;
	};

	// GUI class contains information about widgets to load (layouts)
	// and map of loaded layouts
	// When GUI is activated in GUIManager, it loads all layouts from
	// list. For pop-up windows (like in-game options) it is best to load its layout and keep it
	// and use HideLayout()/ShowLayout to hide/show it
	// When GUI is deactivated all widgets are hidden, so after switching to
	// another GUI and back all events callbacks needn't be set again, but
	// unloading must be handled by other class
	// If gui isn't active and layouts are loaded, they should be hidden and
	// manually shown after setting active gui
	// GUI class shoud be component of other class which will handle
	// input events, add necessary layouts etc.
	class GUI
	{
	protected:
		typedef Array<RefPtr<Layout>> LayoutList;
		LayoutList _loadedLayouts;
		Array<GameGui::GuiElement*> _registeredGuiElements;
		string _name;

	public:
		GUI(const std::string& name);
		~GUI();

		// Updates all gui elements
		void Update(float ms);

		// Loads layout and adds its widgets to gui
		// If layout wasn't added, adds it
		// If parent is specified, adds widgets as children
		ErrorCode LoadLayout(const std::string& layoutName, MyGUI::Widget* parent = 0, bool loadhidden = false);
		// Destroys all widget from given layout
		// If some layout is children of unloaded one, then it is
		// unloaded as well
		// Version with pointer do not perform check if layout is actaully loaded
		ErrorCode UnloadLayout(const std::string& layoutName);
		ErrorCode UnloadLayout(RefPtr<Layout> layout);

		ErrorCode UnloadAllLayouts();

		// Adds loaded layout to gui without parent
		void AddLayout(RefPtr<Layout> layout)
		{
			_loadedLayouts.push_back(layout);
		}


		// Shows all widgets from given layout ( layout must be loaded )
		ErrorCode ShowLayout(const std::string& layoutName);
		// Hides all widgets from given layout ( layout must be loaded )
		ErrorCode HideLayout(const std::string& layoutName);

		ErrorCode HideAllLayouts();
		ErrorCode ShowAllLayouts();

		const string& GetName() const { return _name; }
		const LayoutList& GetLoadedLayouts() const { return _loadedLayouts; }

		RefPtr<Layout> GetLayout(const string& name);

		// Retuns pointer to widget with given name or 0 if not found
		MyGUI::Widget* FindWidget(const std::string& widgetName) const;

		// Returns pointer to widget from layout with given name or 0 if not found
		MyGUI::Widget* FindInLayout(RefPtr<Layout> layout, const string& name) const;

		// Returns pointer to widget from hierarchy or 0 if not found
		MyGUI::Widget* FindChildren(MyGUI::Widget* parent, const string& name) const;

		// Resitere gui element for updates ( GUI doesn't own them )
		void AddGuiElement(GameGui::GuiElement* element);
		void RemoveGuiElement(GameGui::GuiElement* element);
	};
}