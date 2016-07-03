#pragma once 

#include "ObjectDefinition.h"
#include <Xml.h>

namespace Ogre
{
	class Image;
}

namespace FlagRTS
{
	// Definition ofButton that has to be pressed for command to be executed
	// It is shown on unit menu
	class CommandButtonDefinition : public ObjectDefinition
	{
	protected:
		string _iconName;

		string _commandName; // Name of command
		string _commandDesc; // Description of command

		bool _showHotkey; // If true hotkey will be added to descripition
		bool _showEnergy; // If true ability energy usage will be added to descripition 
		bool _showCooldown; // If true ability cooldown will be added to descripition 
		bool _showCost; // If true unit resource cost will be added to descripition 
		bool _showTime; // If true unit build time will be added to descripition 
		bool _showProgress; // If true unit build progress or ability cooldown will be shown as partialy shaded button 

	public:
		CommandButtonDefinition();
		CommandButtonDefinition(XmlNode* cbNode);

		const string& GetCommandName() const { return _commandName; }
		void SetCommandName(const string& name) { _commandName = name; }

		const string& GetCommandDesc() const { return _commandDesc; }
		void SetCommandDesc(const string& desc) { _commandDesc = desc; }

		const string& GetIconName() const { return _iconName; }
		void SetIconName(const string& icon) { _iconName = icon; }

		bool GetShowHotkey() const { return _showHotkey; }
		void SetShowHotkey(bool value) { _showHotkey = value; }

		bool GetShowEnergy() const { return _showEnergy; }
		void SetShowEnergy(bool value) { _showEnergy = value; }

		bool GetShowCooldown() const { return _showCooldown; }
		void SetShowCooldown(bool value) { _showCooldown = value; }

		bool GetShowCost() const { return _showCost; }
		void SetShowCost(bool value) { _showCost = value; }

		bool GetShowTime() const { return _showTime; }
		void SetShowTime(bool value) { _showTime = value; }

		bool GetShowProgress() const { return _showProgress; }
		void SetShowProgress(bool value) { _showProgress = value; }
	};
}