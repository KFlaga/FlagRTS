#include "GUI.h"

#include <MyGUI\MyGUI.h>
#include <MyGUI\MyGUI_OgrePlatform.h>

#include "Configuration.h"
#include "LogManager.h"
#include "ErrorManager.h"
#include "Memory.h"
#include "GuiElement.h"

namespace FlagRTS
{
	GUI::GUI(const std::string& name)
	{
		_name = name;
	}

	GUI::~GUI()
	{
		UnloadAllLayouts();
	}

	void GUI::Update(float ms)
	{
		for(auto eIt = _registeredGuiElements.begin(); eIt != _registeredGuiElements.end(); ++eIt)
		{
			if((*eIt)->IsVisible())
				(*eIt)->Update(ms);
		}
	}

	RefPtr<Layout> GUI::GetLayout(const string& name)
	{
		RefPtr<Layout> layout;
		for(auto layIt = _loadedLayouts.begin(); layIt != _loadedLayouts.end(); layIt++)
		{
			if((*layIt)->Name.compare(name) == 0)
			{
				layout = *layIt;
			}
		}
		return layout;
	}

	ErrorCode GUI::LoadLayout(const std::string& layoutName, 
		MyGUI::Widget* parent , bool loadhidden)
	{
		// Check if layout is already loaded
		for(auto layIt = _loadedLayouts.begin(); layIt != _loadedLayouts.end(); layIt++)
		{
			if((*layIt)->Name.compare(layoutName) == 0)
				return RETURN_ERROR(ErrorCodes::LayoutAlreadyLoaded);
		}

		// Load widgets from file ( only top level widgets are returned, rest
		// is in child hierarchy ) and build layout class
		MyGUI::VectorWidgetPtr widgets = MyGUI::LayoutManager::getInstance().loadLayout(layoutName);
		RefPtr<Layout> layout(xNew0(Layout));
		layout->Name = layoutName;
		layout->Parent = parent;
		layout->Widgets = widgets;

		// Set parent for top-level widgets if such is specified
		if(parent != 0)
		{
			for(auto wit = widgets.begin(); wit != widgets.end(); wit++)
			{
				(*wit)->attachToWidget(parent);
			}
		}

		_loadedLayouts.push_back(layout);
		// Hide widgets if requested
		if(loadhidden)
		{
			for(auto wit = widgets.begin(); wit != widgets.end(); wit++)
			{
				(*wit)->setVisible(false);
			}
		}

		return ErrorCodes::Success;
	}

	ErrorCode GUI::UnloadLayout(const std::string& layoutName)
	{
		// First find layout and remove from list
		RefPtr<Layout> layout;
		for(auto layIt = _loadedLayouts.begin(); layIt != _loadedLayouts.end(); layIt++)
		{
			if((*layIt)->Name.compare(layoutName) == 0)
			{
				layout = *layIt;
				_loadedLayouts.erase(layIt);
				break;
			}
		}

		if(!layout.IsValid()) // Coundn't find layout
			return RETURN_ERROR(ErrorCodes::LayoutNotLoaded);

		UnloadLayout(layout);
		return ErrorCodes::Success;
	}

	ErrorCode GUI::UnloadLayout(RefPtr<Layout> layout)
	{
		// Look for all layouts that contains parents in unloaded layout
		// and remove them from loaded list ( they will be destroyes as well )
		for(auto layIt = _loadedLayouts.begin(); layIt != _loadedLayouts.end();)
		{
			// Check only layouts with parents and omit self check
			if( (*layIt)->Parent != 0 && (*layIt)->Name.compare(layout->Name) != 0)
			{
				MyGUI::Widget* parent = FindInLayout(layout, (*layIt)->Parent->getName());
				if(parent != 0)
				{
					// Layout in layIt have parent in layout, so delete them from list
					// They will be destroyed when parent is destroyed
					// After erase layIt is invalid, so it cant be ++'ed (?)
					// So get next layout from return value, but do not increment it then
					layIt = _loadedLayouts.erase(layIt);
					continue;
				}
			}
			layIt++;
		}

		MyGUI::WidgetManager::getInstance().destroyWidgets(layout->Widgets);
		_loadedLayouts.remove(layout);
		return ErrorCodes::Success;
	}

	ErrorCode GUI::UnloadAllLayouts()
	{
		while(_loadedLayouts.size() > 0)
		{
			UnloadLayout(*_loadedLayouts.begin());
		}

		return ErrorCodes::Success;
	}

	ErrorCode GUI::ShowLayout(const std::string& layoutName)
	{
		RefPtr<Layout> layout = GetLayout(layoutName);

		if(!layout.IsValid()) // Coundn't find layout
			return RETURN_ERROR(ErrorCodes::LayoutNotLoaded);

		for(auto widget = layout->Widgets.begin(); widget != layout->Widgets.end(); widget++)
		{
			(*widget)->setVisible(true);
		}

		return ErrorCodes::Success;
	}

	ErrorCode GUI::HideLayout(const std::string& layoutName)
	{
		RefPtr<Layout> layout = GetLayout(layoutName);

		if(!layout.IsValid()) // Coundn't find layout
			return RETURN_ERROR(ErrorCodes::LayoutNotLoaded);

		for(auto widget = layout->Widgets.begin(); widget != layout->Widgets.end(); widget++)
		{
			(*widget)->setVisible(true);
		}

		return ErrorCodes::Success;
	}

	ErrorCode GUI::HideAllLayouts()
	{
		for(auto it = _loadedLayouts.begin(); it != _loadedLayouts.end(); it++)
		{
			for(auto widget = (*it)->Widgets.begin(); widget != (*it)->Widgets.end(); widget++)
			{
				(*widget)->setVisible(false);
			}
		}
		return ErrorCodes::Success;
	}

	ErrorCode GUI::ShowAllLayouts()
	{
		for(auto it = _loadedLayouts.begin(); it != _loadedLayouts.end(); it++)
		{
			for(auto widget = (*it)->Widgets.begin(); widget != (*it)->Widgets.end(); widget++)
			{
				(*widget)->setVisible(true);
			}
		}
		return ErrorCodes::Success;
	}

	MyGUI::Widget* GUI::FindWidget(const std::string& widgetName) const
	{
		for(auto layout = _loadedLayouts.cbegin(); layout != _loadedLayouts.cend(); layout++)
		{
			// For all parent layouts search widget in layout and children
			if((*layout)->Parent == 0)
			{
				MyGUI::Widget* w = FindInLayout(*layout, widgetName);
				if(w != 0)
					return w;
			}
		}

		return 0;
	}

	MyGUI::Widget* GUI::FindInLayout(RefPtr<Layout> layout, const string& name) const
	{
		for(auto widget = layout->Widgets.begin(); widget != layout->Widgets.end(); widget++)
		{
			if((*widget)->getName().compare(name) == 0)
			{
				return *widget;
			}
			else
			{
				MyGUI::Widget* w = FindChildren(*widget, name);
				if(w != 0)
				{
					return w;
				}
			}
		}

		return 0;
	}

	MyGUI::Widget* GUI::FindChildren(MyGUI::Widget* parent, const string& name) const
	{
		for(uint32 i = 0; i < parent->getChildCount(); i++)
		{
			if(parent->getChildAt(i)->getName().compare(name) == 0)
			{
				return parent->getChildAt(i);
			}
			else
			{
				MyGUI::Widget* w = FindChildren(parent->getChildAt(i),name);
				if( w!= 0)
					return w;
			}
		}
		return 0;
	}

	void GUI::AddGuiElement(GameGui::GuiElement* element)
	{
		_registeredGuiElements.push_back(element);
	}

	void GUI::RemoveGuiElement(GameGui::GuiElement* element)
	{
		_registeredGuiElements.remove(element);
	}
}