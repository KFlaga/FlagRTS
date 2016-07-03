#pragma once

#include <MyGUI\MyGUI.h>
#include "TypeDefs.h"

namespace FlagRTS
{
	// Cursor stores image sets for cursor, for different states or
	// hover objects (like normal arrow, waiting, hover player, hover enemy)
	// Cursor object is changed if ie. player is using ability or order to move/attack
	// 
	class CursorContext : public MyGUI::IResource
	{
		MYGUI_RTTI_DERIVED(CursorContext)
	private:
		// Names of cursors for different states. Every cursor should handle at least "Default" state
		MyGUI::MapString _cursorsForStates; 

	public:
		// Loads cursor data from xml node
		virtual void deserialization(MyGUI::xml::ElementPtr xmlNode, MyGUI::Version version);

		// Return string with cursor name for given state
		// If state not found returns onr for 'Default' state
		string GetCursorForState(const string& stateName);
	};
}