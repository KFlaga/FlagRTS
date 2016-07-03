#pragma once

#include <GUI.h>

namespace FlagRTS
{
	class IOptionsPanel
	{
	protected:
		RefPtr<GUI> _gui; // Parent gui with options screen
		MyGUI::Widget* _parentPanel; // Main panel of options screen
		string _name;
	public:
		// ParentGui - pointer to Options gui
		// ParentPanel - pointer to main background options panel
		IOptionsPanel(RefPtr<GUI> parentGui, MyGUI::Widget* parentPanel, string name) : 
			_gui(parentGui), 
			_parentPanel(parentPanel),
			_name(name)
		{

		}

		virtual ~IOptionsPanel() { }

		string GetName() const
		{
			return _name;
		}

		// Hides panel
		virtual void Hide() = 0;
		// Shows panel
		virtual void Show() = 0;
		// Saves changes in settings ( changes are stored in local structures
		// and saved to actual configs after in this call )
		virtual void SaveSettings() = 0;
		// Makes controls return to state from last save ( state in configs )
		virtual void UndoSettings() = 0;
		// Unloads panel layouts
		virtual void Unload() = 0;

		virtual ErrorCode Update(float ms) = 0;
	};
}