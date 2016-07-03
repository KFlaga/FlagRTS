#include "CommandButtonDefinition.h"
#include "CommandButton.h"
#include <OgreImage.h>
#include <LocalizationManager.h>

namespace FlagRTS
{
	CommandButtonDefinition::CommandButtonDefinition() :
		ObjectDefinition()
	{
		SetFinalType(GetTypeId<CommandButton>());
		SetFinalTypeName("CommandButton");
	}

	CommandButtonDefinition::CommandButtonDefinition(XmlNode* cbNode) :
		ObjectDefinition(cbNode)
	{
		SetFinalType(GetTypeId<CommandButton>());
		SetFinalTypeName("CommandButton");

		_iconName = XmlUtility::XmlGetString(cbNode->first_node("Icon",4), "name", 4);
		// Read button descriptions
		_commandName = XmlUtility::XmlGetLocaleString(cbNode->first_node("CommandName",11));
		_commandDesc = XmlUtility::XmlGetLocaleString(cbNode->first_node("CommandDesc",11));

		// Read flags
		XmlNode* flagsNode = cbNode->first_node("Flags", 5);
		_showHotkey = XmlUtility::XmlGetFlag(flagsNode, "ShowHotkey", false, 10, true);
		_showEnergy = XmlUtility::XmlGetFlag(flagsNode, "ShowEnergy", false, 10, true);
		_showCooldown = XmlUtility::XmlGetFlag(flagsNode, "ShowCooldown", false, 10, true);
		_showCost = XmlUtility::XmlGetFlag(flagsNode, "ShowCost", false, 10, true);
		_showTime = XmlUtility::XmlGetFlag(flagsNode, "ShowTime", false, 10, true);
		_showProgress = XmlUtility::XmlGetFlag(flagsNode, "ShowProgress", false, 10, true);
	}
}
