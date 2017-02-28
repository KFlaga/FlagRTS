#include "UnitDefinition.h"
#include "Unit.h"
#include "UnitStats.h"
#include "UnitCommandSet.h"
#include "SelectionFlags.h"
#include "UnitClass.h"
#include "IObjectDefinitionManager.h"
#include "GameWorld.h"
#include <SystemSettings.h>
#include "CommandButton.h"
#include <KeyBindings.h>
#include "UnitSoundSet.h"
#include <AudioManager.h>
#include <SoundSource.h>
#include <SoundChannel.h>
#include <StringUtils.h>
#include "SceneMarker.h"
#include "IMoveStrategy.h"

namespace FlagRTS
{
	UnitDefinition::UnitDefinition() :
		PhysicalObjectDefinition(),
		_baseStats(xNew0(UnitStats)),
		_countsAsPlayerUnit(false),
		_defaultMoveStrategy(0)
	{
		SetFinalType(GetTypeId<Unit>());
		SetFinalTypeName("Unit");
	}

	UnitDefinition::UnitDefinition(XmlNode* soDefNode) :
		PhysicalObjectDefinition(soDefNode),
		_baseStats(xNew0(UnitStats))
	{
		SetFinalType(GetTypeId<Unit>());
		SetFinalTypeName("Unit");

		// Get node specific to Unit
		XmlNode* unitNode = soDefNode->first_node("PhysicalObject")->
			first_node("Unit");

		XmlNode* propsNode = unitNode->first_node("Properties");
		if(propsNode != 0)
			ParseUnitProperties(propsNode);

		XmlNode* soundsNode = unitNode->first_node("SoundSet");
		ParseUnitSounds(soundsNode);

		_baseStats->Parse(unitNode->first_node("Stats",5));

		_iconName = XmlUtility::XmlGetString(unitNode->first_node("Icon",4), "name", 4);
		_guiName = XmlUtility::XmlGetLocaleString(unitNode->first_node("GuiName",7));
		/*
		Ogre::Image icon;
		icon.load(_iconName, "Icons");
		Ogre::TexturePtr iconTexture = Ogre::TextureManager::getSingleton().createManual(
		_iconName, "Icons", Ogre::TEX_TYPE_2D, (int)icon.getWidth(), (int)icon.getHeight(), 1, icon.getFormat());
		iconTexture->loadImage(icon);
		*/
		ParseCommands(unitNode->first_node("Commands"));

		ParseMoveStrategy(unitNode->first_node("MoveStrategy"));
	}

	UnitDefinition::~UnitDefinition()
	{
		xDelete(_baseStats);
		/*for(unsigned int i = 0; i < _trainableUnitsDefs.size(); ++i)
		{
			xDelete( _trainableUnitsDefs[i]->DefinitionName );
			for(unsigned int req = 0; req < _trainableUnitsDefs[i]->RequirementNames.size(); ++req)
			{
				xDelete( _trainableUnitsDefs[i]->RequirementNames[req] );
			}
		}*/
	}

	void UnitDefinition::ParseUnitProperties(XmlNode* propsNode)
	{
		_countsAsPlayerUnit = XmlUtility::XmlGetFlag(propsNode, "CountsAsUnit", true);
	}

	void UnitDefinition::ParseCommands(XmlNode* commandsNode)
	{
		// Get command list
		XmlNode* commandNode = commandsNode->first_node("Command",7);
		while(commandNode != 0)
		{
			Command* customCommand = 0;
			// Read command type and if its default one
			int key = OrderTypes::ParseOrderType(commandNode->first_attribute("type",4,false)->value());
			if( XmlUtility::XmlGetBool(commandNode, "custom", 6) )
			{
				// Custom command ( ability or non-default non-ability )
			}

			// Get command button its name, position, hotkey
			XmlNode* buttonNode = commandNode->first_node("Button",6);

			string buttonName = XmlUtility::XmlGetString(buttonNode,"definition", 10);
			int hotkey = SystemSettings::GetKeyBindings().KeyCodeNamesMap[
				XmlUtility::XmlGetString(buttonNode, "key", 3)];
				int posx = XmlUtility::XmlGetInt(buttonNode, "x", 1);
				int posy = XmlUtility::XmlGetInt(buttonNode, "y", 1);

				// Lambda funtion for creating button -> button needs definition which can
				// be not loaded at this point ( as CommandSet is created in UnitDefinition ),
				// so we'll create it later, when all Definitions are loaded
				auto createButton = [this, key, buttonName, hotkey, posx, posy, customCommand](IObjectDefinitionManager* mgr) 
				{
					CommandButton* button = xNew1(CommandButton, static_cast<CommandButtonDefinition*>(
						mgr->GetObjectDefinitionByName(buttonName, "CommandButton")));
					button->SetHotkey(hotkey);
					button->SetPosX(posx);
					button->SetPosY(posy);
					if( customCommand == 0 )
						_defaultCommandButtons.push_back(std::make_pair(key, button));
					else
						_customCommands.push_back(std::make_pair(customCommand, button));
				};
				typedef DelegateEventHandler<decltype(createButton), IObjectDefinitionManager*> DefinitionsLoadedHandler;
				GameInterfaces::GetObjectDefinitionManager()->OnAllDefinitionsLoaded() +=
					xNew1(DefinitionsLoadedHandler, createButton);

				commandNode = commandNode->next_sibling();
		}
	}

	void UnitDefinition::ParseUnitSounds(XmlNode* soundsNode)
	{
		_unitSoundSet.resize( UnitSounds::DefSoundActionsCount, 0 );

		if( soundsNode != 0 )
		{
			// Parse all sounds
			XmlNode* sndNode = soundsNode->first_node("Sound");
			while(sndNode != 0)
			{
				const char* actionName = XmlUtility::XmlGetString(sndNode, "action");
				const char* soundName = XmlUtility::XmlGetString(sndNode, "name");
				const char* soundPack = XmlUtility::XmlGetString(sndNode, "pack");
				float gain = XmlUtility::XmlGetFloatIfExists(sndNode, "gain", 1.f);

				int actionNum = UnitSounds::ParseSoundAction(actionName);
				_ASSERT( actionNum >= 0 );
				if( actionNum < 0 )
					continue;

				Media::SoundSource* ssource = Media::AudioManager::Instance()->CreateSoundSource(
					soundName, soundPack);

				_unitSoundSet[actionNum] = ssource;
				ssource->SetGain(gain);

				Media::AudioManager::Instance()->LoadSoundPack( soundPack );

				sndNode = sndNode->next_sibling();
			}
		}

		// For all non-set sounds set default ones
		for(unsigned int i = 0; i < _unitSoundSet.size(); ++i)
		{
			if( _unitSoundSet[i] == 0 )
			{
				Media::SoundSource* ssource = Media::AudioManager::Instance()->CreateSoundSource(
					UnitSounds::GetDefaultSound((UnitSounds::SoundAction)i));
				_unitSoundSet[i] = ssource;
			}
		}
	}

	void UnitDefinition::ParseMoveStrategy(XmlNode* moveNode)
	{
		string moveStrategyName;
		if(moveNode == 0)
		{
			// Unit doesn't move
			moveStrategyName = "NoMove";
		}
		else
		{
			moveStrategyName = XmlUtility::XmlGetString(moveNode, "name");
		}

		auto getDefinition = [this, moveStrategyName](IObjectDefinitionManager* mgr) 
		{
			_defaultMoveStrategy = static_cast<IMoveStrategy*>(
				mgr->GetObjectDefinitionByName("MoveStrategy", moveStrategyName));
		};
		typedef DelegateEventHandler<decltype(getDefinition), IObjectDefinitionManager*> DefinitionsLoadedHandler;
		GameInterfaces::GetObjectDefinitionManager()->OnAllDefinitionsLoaded() +=
			xNew1(DefinitionsLoadedHandler, getDefinition);
	}
}