#include "PlayerPanel.h"
#include <MyGUI\MyGUI.h>
#include "CreateGameScreen.h"
#include <Exception.h>

namespace FlagRTS
{

	PlayerPanel::PlayerPanel(RefPtr<GUI> gui, RefPtr<Layout> layout, int player) :
		_player(player),
		_isReady(false),
		_color(player),
		_faction(-1),
		_team(-1),
		_startLoc(-1),
		_slot(SlotPlayers::Closed),
		_slotChangeInternal(false),
		_factionChangeInternal(false)
	{
		string playerName = string("Player") + std::to_string(player);
		_panel = static_cast<MyGUI::Widget*>(gui->FindInLayout(layout, string("Panel") + playerName));
		_comboSlot = static_cast<MyGUI::ComboBox*>(_panel->findWidget(string("ComboSlot_") + playerName));
		_checkReady = static_cast<MyGUI::Button*>(_panel->findWidget(string("CheckReady_") + playerName));
		_colorBox = static_cast<MyGUI::Button*>(_panel->findWidget(string("Color_") + playerName));
		_comboFaction = static_cast<MyGUI::ComboBox*>(_panel->findWidget(string("ComboFaction_") + playerName));
		_buttonTeam = static_cast<MyGUI::Button*>(_panel->findWidget(string("ButtonTeam_") + playerName));
		_labelTeam = static_cast<MyGUI::EditBox*>(_panel->findWidget(string("LabelTeam_") + playerName));
		_buttonStartLoc = static_cast<MyGUI::Button*>(_panel->findWidget(string("ButtonStartLoc_") + playerName));
		_labelStartLoc = static_cast<MyGUI::EditBox*>(_panel->findWidget(string("LabelStartLoc_") + playerName));

		auto& color = CreateGameScreen::PlayersColorsList[CreateGameScreen::NoColorIndex];
		_colorBox->setColour(MyGUI::Colour(color.x, color.y, color.z));

		_comboSlot->setCaption("");
		_comboSlot->setEnabled(false);
		_checkReady->setEnabled(false);
		_colorBox->setEnabled(false);
		_comboFaction->setCaption("");
		_comboFaction->setEnabled(false);
		_buttonTeam->setEnabled(false);
		_buttonStartLoc->setEnabled(false);
		_panel->setVisible(false);

		_comboSlot->eventComboChangePosition += MyGUI::newDelegate(
			this, &PlayerPanel::SlotSelected);
		_comboFaction->eventComboChangePosition += MyGUI::newDelegate(
			this, &PlayerPanel::FactionSelected);
		_checkReady->eventMouseButtonClick += MyGUI::newDelegate(
			this, &PlayerPanel::ReadyButtonClicked);
		_buttonTeam->eventMouseButtonPressed += MyGUI::newDelegate(
			this, &PlayerPanel::TeamButtonClicked);
		_buttonStartLoc->eventMouseButtonPressed += MyGUI::newDelegate(
			this, &PlayerPanel::StartLocButtonClicked);
		_colorBox->eventMouseButtonPressed += MyGUI::newDelegate(
			this, &PlayerPanel::ColorButtonClicked);
	}

	void PlayerPanel::SetIsAvailable(bool value)
	{
		if( value == false )
		{
			_checkReady->setEnabled(false);
			_checkReady->setStateCheck(false);
			_colorBox->setEnabled(false);
			_comboFaction->setCaption("");
			_comboFaction->setEnabled(false);
			_buttonTeam->setEnabled(false);
			_buttonStartLoc->setEnabled(false);
			_isReady = false;
			_comboSlot->setCaption("Not Available");
			_comboSlot->setEnabled(false);
			SetColor(CreateGameScreen::NoColorIndex);
		}
		else
		{
			_comboSlot->setEnabled(true);
			SetSlot(SlotPlayers::Closed);
		}
		_isAvailable = value;
	}

	void PlayerPanel::SetSlot(int value)
	{
		if( value != _slot )
		{
			if( value == SlotPlayers::Closed )
			{
				// Close -> disable controls
				_comboSlot->setIndexSelected(0);
				_checkReady->setEnabled(false);
				_colorBox->setEnabled(false);
				_comboFaction->setEnabled(false);
				_buttonTeam->setEnabled(false);
				_buttonStartLoc->setEnabled(false);
			}
			else
			{
				// Was disabled -> enable
				_checkReady->setEnabled(true);
				_colorBox->setEnabled(true);
				_comboFaction->setEnabled(true);
				_buttonTeam->setEnabled(true);
				_buttonStartLoc->setEnabled(true);

				if( value == SlotPlayers::Human )
				{
					_comboSlot->setCaption("Player");
				}
				else if( value == SlotPlayers::AI )
				{
					_comboSlot->setCaption("AI");
				}
			}
			_slot = value;
			SetIsReady(false);
		}
	}

	void PlayerPanel::SetIsReady(bool value)
	{
		if( value == _isReady )
			return;

		if( _isReady )
		{
			// Unready player and enable controls
			_isReady = false;
			_checkReady->setStateCheck(false);
			_buttonTeam->setEnabled(true);
			_buttonStartLoc->setEnabled(true);
			_colorBox->setEnabled(true);
			_comboFaction->setEnabled(true);
			_comboSlot->setEnabled(true);
			_playerIsReadyChanged.Fire(_player, _isReady);
			return;
		}

		// Check if player can be ready -> all options must be set
		if( _comboSlot->getIndexSelected() != SlotPlayers::Closed &&
			_team != -1 &&
			_startLoc != -1 &&
			_faction != -1 )
		{
			// Set player ready and disable controls
			_isReady = true;
			_checkReady->setStateCheck(true);
			_buttonTeam->setEnabled(false);
			_buttonStartLoc->setEnabled(false);
			_colorBox->setEnabled(false);
			_comboFaction->setEnabled(false);
			_comboSlot->setEnabled(false);
			_playerIsReadyChanged.Fire(_player, _isReady);
		}
	}

	void PlayerPanel::SetColor(int value)
	{
		_color = value;
		auto& color = CreateGameScreen::PlayersColorsList[value];
		_colorBox->setColour(MyGUI::Colour(color.x, color.y, color.z));
	}

	bool PlayerPanel::SetFaction(const string& value)
	{
		_team = -1;
		if( value != "")
		{
			for(unsigned int i = 0; i < _availableFactions.size(); ++i)
			{
				if(_availableFactions[i] == value)
				{
					_comboFaction->setCaption(value);
					_comboFaction->setEnabled(true);
					_faction = i;
					return true;
				}
			}
			_comboFaction->setCaption("");
			_comboFaction->setEnabled(false);
			return false;
		}
		else
		{
			_comboFaction->setCaption("");
			_comboFaction->setEnabled(false);
			return true;
		}
	}

	bool PlayerPanel::SetTeam(int value)
	{
		_team = -1;
		if( value >= 0)
		{
			for(unsigned int i = 0; i < _availableTeams.size(); ++i)
			{
				if(_availableTeams[i] == value)
				{
					_labelTeam->setCaption(std::to_string(value));
					_buttonTeam->setEnabled(true);
					_team = i;
					return true;
				}
			}
			_labelTeam->setCaption("");
			_buttonTeam->setEnabled(false);
			return false;
		}
		else
		{
			_labelTeam->setCaption("");
			_buttonTeam->setEnabled(false);
			return true;
		}
	}

	bool PlayerPanel::SetStartPosition(int value)
	{
		_startLoc = -1;
		if( value >= 0)
		{
			for(unsigned int i = 0; i < _availableStartLocs.size(); ++i)
			{
				if(_availableStartLocs[i] == value)
				{
					_labelStartLoc->setCaption(std::to_string(value));
					_buttonStartLoc->setEnabled(true);
					_startLoc = i;
					return true;
				}
			}
			return false;
		}
		else
		{
			_labelStartLoc->setCaption("");
			_buttonStartLoc->setEnabled(false);
			return true;
		}
	}

	void PlayerPanel::SetMap(XmlNode* mapNode, XmlNode* modeNode)
	{
		// 1) Reset
		ResetRestrictions();
		_slot = SlotPlayers::Closed;

		// 2) Set player specific restrictions
		XmlNode* playerNode = FindPlayerNode(modeNode);
		if( playerNode != 0 )
		{
			// Get player restrictions
			// 1) Check availability
			_isAvailable = XmlUtility::XmlGetBoolIfExists(playerNode, "available", true);

			// 2) Check if can be human/ai
			_canBeHuman = XmlUtility::XmlGetBoolIfExists(playerNode, "can_be_human", true);
			_canBeAi = XmlUtility::XmlGetBoolIfExists(playerNode, "can_be_ai", true);

			CheckFactionsRestriction(playerNode);
			CheckStartLocRestriction(playerNode);
			CheckTeamRestriction(playerNode);
		}

		// 3) If some are not there, look for mode-specific restrictions
		if( _factionsRestricted == false )
			CheckFactionsRestriction(modeNode);
		if( _startLocRestricted == false )
			CheckStartLocRestriction(modeNode);
		if( _teamsRestricted == false )
			CheckTeamRestriction(modeNode);

		// 4) If still some remains, set map-specific restrictions
		if( _factionsRestricted == false )
			CheckFactionsRestriction(mapNode);
		if( _startLocRestricted == false )
			CheckStartLocRestriction(mapNode);
		if( _teamsRestricted == false )
			CheckTeamRestriction(mapNode);

		// 5) Finnaly update changes
		UpdateControls();
	}

	void PlayerPanel::ResetRestrictions()
	{
		_availableFactions.clear();
		_availableStartLocs.clear();
		_availableTeams.clear();
		_factionsRestricted = false;
		_startLocRestricted = false;
		_teamsRestricted = false;
		_isAvailable = true;
		_canBeAi = true;
		_canBeHuman = true;
	}

	void PlayerPanel::CheckFactionsRestriction(XmlNode* node)
	{
		XmlNode* factionListNode = node->first_node("AvailableFactions");
		if( factionListNode != 0 )
		{
			_factionsRestricted = true;
			XmlNode* factionNode = factionListNode->first_node();
			while(factionNode != 0)
			{
				_availableFactions.push_back(
					XmlUtility::XmlGetString(factionNode, "name"));
				factionNode = factionNode->next_sibling();
			}
		}
	}

	void PlayerPanel::CheckStartLocRestriction(XmlNode* node)
	{
		XmlNode* startLocListNode = node->first_node("AvailableStartLocations");
		if( startLocListNode != 0 )
		{
			_startLocRestricted = true;
			XmlNode* startLocNode = startLocListNode->first_node();
			int startLocNum = 0;
			while(startLocNode != 0)
			{
				_availableStartLocs.push_back(
					XmlUtility::XmlGetInt(startLocNode, "id"));
				startLocNode = startLocNode->next_sibling();
			}

		}
	}

	void PlayerPanel::CheckTeamRestriction(XmlNode* node)
	{
		XmlNode* teamListNode = node->first_node("AvailableTeams");
		if( teamListNode != 0 )
		{
			_teamsRestricted = true;
			XmlNode* teamNode = teamListNode->first_node();
			while(teamNode != 0)
			{
				_availableTeams.push_back(
					XmlUtility::XmlGetInt(teamNode, "id"));
				teamNode = teamNode->next_sibling();
			}
		}
	}

	XmlNode* PlayerPanel::FindPlayerNode(XmlNode* modeNode)
	{
		XmlNode* playerListNode = modeNode->first_node("Players");
		if( playerListNode == 0 )
		{
			return 0;
		}
		XmlNode* playerNode = playerListNode->first_node();
		while(playerNode != 0)
		{
			if( XmlUtility::XmlGetIntIfExists(playerNode, "id", -1) == _player )
			{
				// Found player node
				break;
			}

			playerNode = playerNode->next_sibling();
		}
		return playerNode;
	}

	void PlayerPanel::UpdateControls()
	{
		// In this moment, panel is Closed, so disable some control
		_checkReady->setEnabled(false);
		_colorBox->setEnabled(false);
		_comboFaction->setCaption("");
		_comboFaction->setEnabled(false);
		_buttonTeam->setEnabled(false);
		_buttonStartLoc->setEnabled(false);
		_isReady = false;

		// 1) Check panel availability
		if( _isAvailable == false )
		{
			// Disable all controls
			_comboSlot->setCaption("Not Available");
			_comboSlot->setEnabled(false);
			_labelTeam->setCaption("");
			_labelStartLoc->setCaption("");
			return;
		}

		_comboSlot->setEnabled(true);

		// 2) Feed slot combo
		_comboSlot->removeAllItems();
		_comboSlot->addItem("Close", (int)SlotPlayers::Closed);

		if( _canBeHuman )
		{
			_comboSlot->addItem("Player", (int)SlotPlayers::Human);
		}
		if( _canBeAi )
		{
			_comboSlot->addItem("AI", (int)SlotPlayers::AI);
		}
		_comboSlot->setIndexSelected(0);
		_slot = SlotPlayers::Closed;

		// Feed controls with available options
		_comboFaction->removeAllItems();
		for(unsigned int i = 0; i < _availableFactions.size(); ++i)
		{
			_comboFaction->addItem(
				_availableFactions[i], (int)i);
		}
		_comboFaction->setIndexSelected(0);
		_faction = 0;
		SetTeam(_availableTeams[_player % _availableTeams.size()]);
		_buttonTeam->setEnabled(false);
		SetStartPosition(-1);
		SetColor(CreateGameScreen::NoColorIndex);
	}

	void PlayerPanel::Show()
	{
		_panel->setVisible(true);
	}

	void PlayerPanel::Hide()
	{
		_panel->setVisible(false);
	}

	void PlayerPanel::Resize(float horizontalRatio, float verticalRatio)
	{

	}

	void PlayerPanel::ColorButtonClicked(MyGUI::Widget*, int x, int y, MyGUI::MouseButton button)
	{
		if( button == MyGUI::MouseButton::Right )
		{
			_colorChangeRequested.Fire(_player, 
				_color > 0 ? _color-1 : (int)CreateGameScreen::PlayersColorsList.size() - 2);
		}
		else if( button == MyGUI::MouseButton::Left )
		{
			_colorChangeRequested.Fire(_player, 
				_color < (int)CreateGameScreen::PlayersColorsList.size() - 2 ? _color+1 : 0);
		}

	}

	void PlayerPanel::TeamButtonClicked(MyGUI::Widget*, int x, int y, MyGUI::MouseButton button)
	{
		if( button == MyGUI::MouseButton::Right )
		{
			_teamChangeRequested.Fire(_player, 
				_team > 0 ? 
				_availableTeams[_team-1] : 
			_availableTeams[_availableTeams.size() - 1]);
		}
		else if( button == MyGUI::MouseButton::Left )
		{
			_teamChangeRequested.Fire(_player, 
				_team < (int)_availableTeams.size() - 1 ? 
				_availableTeams[_team+1] : _availableTeams[0]);
		}
	}

	void PlayerPanel::StartLocButtonClicked(MyGUI::Widget*, int x, int y, MyGUI::MouseButton button)
	{
		if( button == MyGUI::MouseButton::Right )
		{
			_startLocChangeRequested.Fire(_player, 
				_startLoc > 0 ? 
				_availableStartLocs[_startLoc-1] : 
			_availableStartLocs[_availableStartLocs.size() - 1]);
		}
		else if( button == MyGUI::MouseButton::Left )
		{
			_startLocChangeRequested.Fire(_player, 
				_startLoc < (int)_availableStartLocs.size() - 1 ? 
				_availableStartLocs[_startLoc+1] : _availableStartLocs[0]);
		}
	}

	void PlayerPanel::ReadyButtonClicked(MyGUI::Widget*)
	{
		SetIsReady( !_isReady );
	}

	void PlayerPanel::SlotSelected(MyGUI::ComboBox* sender, size_t index)
	{
		if( *_comboSlot->getItemDataAt<int>(index, false) == _slot )
			return;

		if( _slotChangeInternal == true )
		{
			// Function is not becouse of user input -> ignore
			return;
		}
		// Fire change slot request and return to previous selection
		_slotChangeInternal = true;
		for(unsigned int i = 0; i < _comboSlot->getItemCount(); ++i)
		{
			if(*_comboSlot->getItemDataAt<int>(index, false) == _slot)
			{
				_comboSlot->setIndexSelected(i);
				break;
			}
		}
		_slotChangeRequested.Fire(_player, *_comboSlot->getItemDataAt<int>(index, false));
		_slotChangeInternal = false;
	}

	void PlayerPanel::FactionSelected(MyGUI::ComboBox* sender, size_t index)
	{
		if( _faction == index )
			return;

		if( _factionChangeInternal == true )
		{
			// Function is not becouse of user input -> ignore
			return;
		}
		// Fire change faction request and return to previous selection
		_factionChangeInternal = true;
		_comboFaction->setIndexSelected(_faction);
		_factionChangeRequested.Fire(_player, _availableFactions[index]);
		_factionChangeInternal = false;
	}
}
