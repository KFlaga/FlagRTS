#pragma once

#include <Xml.h>
#include "DataTypes.h"

namespace FlagRTS
{
	class CommandTarget;
	
	// Defines type of hover target and cursor associated with it based on
	// currently selected units or choosen command
	class HoverContext
	{
	protected:
		bool _customLeftAction;
		string _cursorName;
		int _targetType;
		UnitClass _groupClass; // Many HCs depends on selected unitgroup class

	public:
		HoverContext();
		~HoverContext();

		// Updates info on cursor/commands based on given target
		// Stores target type inside CommandTarget class
		// Info on cursor should be read afterwards with GetCursor
		virtual void UpdateHoverTarget(CommandTarget* target) = 0;

		// Gets cursor name according to hovered object and context
		const string& GetCursor() const { return _cursorName; }

		// Executes custom action linked to mouse input ( except right click )
		virtual void ExecuteLeftAction(CommandTarget* target) { }

		bool CustomLeftAction() const { return _customLeftAction; }
		void SetCustomLeftAction(bool value) { _customLeftAction = value; }
	
		UnitClass GetGroupClass() const { return _groupClass; }
		void SetGroupClass(UnitClass gc) { _groupClass = gc; }

	};

	class CustomHoverContext : public HoverContext
	{
	public:
		CustomHoverContext(XmlNode* ccNode);
	};
}