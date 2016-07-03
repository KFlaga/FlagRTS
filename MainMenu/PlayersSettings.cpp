#include "PlayersSettings.h"
#include "PlayerPanel.h"
#include "CreateGameScreen.h"
#include "MapPreview.h"

namespace FlagRTS
{
	PlayersSettings::PlayersSettings(CreateGameScreen* parent, 
		RefPtr<GUI> gui, RefPtr<Layout> layout) :
	_parentScreen(parent),
		_humanPlayerAssignment(-1),
		_startLocCount(0),
		_onStartLocChangeRequested(this),
		_onTeamChangeRequested(this),
		_onFactionChangeRequested(this),
		_onSlotChangeRequested(this),
		_onColorChangeRequested(this),
		_onPlayerIsReadyChanged(this),
		_allReady(false)
	{

		for(int i = 0; i < 8; ++i)
		{
			PlayerPanel* panel = xNew3(PlayerPanel, gui, layout, i);

			panel->StartLocChangeRequested() += &_onStartLocChangeRequested;
			panel->TeamChangeRequested() += &_onTeamChangeRequested;
			panel->SlotChangeRequested() += &_onSlotChangeRequested;
			panel->FactionChangeRequested() += &_onFactionChangeRequested;
			panel->ColorChangeRequested() += &_onColorChangeRequested;
			panel->PlayerIsReadyChanged() += &_onPlayerIsReadyChanged;

			_playerPanels.push_back(panel);
		}
		_panelPlayers = static_cast<MyGUI::Widget*>(gui->FindInLayout(layout, "PanelSettings"));
		_colorAssignments.resize(CreateGameScreen::PlayersColorsList.size()-1, -1);
	}

	PlayersSettings::~PlayersSettings()
	{
		for(unsigned int i = 0; i < _playerPanels.size(); ++i)
		{
			xDelete(_playerPanels[i]);
		}
	}

	void PlayersSettings::Show()
	{
		_panelPlayers->setVisible(true);
		for(unsigned int i = 0; i < _playerPanels.size(); ++i)
		{
			_playerPanels[i]->Show();
		}
	}

	void PlayersSettings::Hide()
	{
		for(unsigned int i = 0; i < _playerPanels.size(); ++i)
		{
			_playerPanels[i]->Hide();
		}
		_panelPlayers->setVisible(false);
	}

	void PlayersSettings::Resize(float hRatio, float vRatio)
	{

	}

	void PlayersSettings::SetMode(XmlNode* mapNode, const string& modeName)
	{
		ResetColorAssignments();
		_humanPlayerAssignment = -1;
		_startLocCount = 0;
		_allReady = false;
		// Read start locations count
		XmlNode* startLocListNode = mapNode->first_node("AvailableStartLocations");
		if( startLocListNode != 0 )
		{
			XmlNode* startLocNode = startLocListNode->first_node();
			while(startLocNode != 0)
			{
				++_startLocCount;
				startLocNode = startLocNode->next_sibling();
			}
		}

		_startLocAssignments.clear();
		_startLocAssignments.resize(_startLocCount, -1);

		XmlNode* modeNode = FindModeNode(mapNode, modeName);
		// Check if player count is resticted by mode
		_maxPlayers = XmlUtility::XmlGetIntIfExists(modeNode->first_node("Players"), "max_count", -1);
		if( _maxPlayers == -1)
		{
			// So players count is map-specific
			_maxPlayers = XmlUtility::XmlGetInt(mapNode->first_node("Players"), "value");
		}

		// Set map for used player panels
		for(int i = 0; i < _maxPlayers; ++i)
		{
			_playerPanels[i]->SetMap(mapNode, modeNode);
		}
		// Disable rest
		for(int i = _maxPlayers; i < 8; ++i)
		{
			_playerPanels[i]->SetIsAvailable(false);
		}
	}

	XmlNode* PlayersSettings::FindModeNode(XmlNode* mapNode, const string& targetModeName)
	{
		XmlNode* modeListNode = mapNode->first_node("Modes");
		XmlNode* modeNode = modeListNode->first_node();
		while(modeNode != 0)
		{
			string modeName = modeNode->name();
			if( modeName.compare(targetModeName) == 0 )
				break; // Found Mode

			modeNode = modeNode->next_sibling();
		}
		_ASSERT(modeNode != 0);
		return modeNode;
	}

	void PlayersSettings::AssignStartLocation(int player, int location)
	{
		if(player != -1 && _playerPanels[player]->GetIsReady() == true)
		{
			// Assing new location only if player is not ready
			return;
		}

		if(location != -1)
		{
			_startLocAssignments[location] = player;
			if( player != -1 )
			{
				_parentScreen->GetMapPreview()->SetStartingPositionColor(location, 
					CreateGameScreen::PlayersColorsList[_playerPanels[player]->GetColor()]);
				_playerPanels[player]->SetStartPosition(location);
			}
			else
			{
				_parentScreen->GetMapPreview()->SetStartingPositionColor(location, 
					CreateGameScreen::PlayersColorsList[CreateGameScreen::NoColorIndex]);
			}
		}
		else
		{
			DeassignStartLocation(_playerPanels[player]->GetStartPosition());
			_playerPanels[player]->SetStartPosition(-1);
		}
	}

	void PlayersSettings::DeassignStartLocation(int location)
	{
		if(location != -1)
		{
			int player = _startLocAssignments[location];
			if(player != -1 && _playerPanels[player]->GetIsReady() == false)
			{
				// Deassing start location only if player is not ready, reset player panel/map preview
				_startLocAssignments[location] = -1;
				_parentScreen->GetMapPreview()->SetStartingPositionColor(location, 
					CreateGameScreen::PlayersColorsList[CreateGameScreen::NoColorIndex]);
				_playerPanels[player]->SetStartPosition(-1);
			}
		}
	}

	void PlayersSettings::ResetStartLocationAssignments()
	{
		for(unsigned int i = 0; i < _startLocAssignments.size(); ++i)
		{
			_startLocAssignments[i] = -1;
		}
		for(unsigned int i = 0; i < 8; ++i)
		{
			_parentScreen->GetMapPreview()->SetStartingPositionColor(i, 
				CreateGameScreen::PlayersColorsList[CreateGameScreen::NoColorIndex]);
			_playerPanels[i]->SetStartPosition(-1);
		}
	}

	void PlayersSettings::AssignNextPlayerToStartLocation(int location)
	{
		if(location != -1)
		{
			int nextPlayer = GetNextNotAssignedPlayer();
			if( nextPlayer != -1 )
			{
				AssignStartLocation(nextPlayer, location);
			}
		}
	}

	void PlayersSettings::SwitchStartLocationAssignments(int loc1, int loc2)
	{
		_ASSERT(loc1 != -1);
		_ASSERT(loc2 != -1);
		int player1 = _startLocAssignments[loc1];
		int player2 = _startLocAssignments[loc2];
		if( (player1 != -1 && _playerPanels[player1]->GetIsReady() == true) ||
			(player2 != -1 && _playerPanels[player2]->GetIsReady() == true) )
		{
			// If any of switching players is already ready, cancel switching
			return;
		}
		AssignStartLocation(player2, loc1);
		AssignStartLocation(player1, loc2);
	}

	int PlayersSettings::GetNextNotAssignedPlayer()
	{
		for(int i = 0; i < _maxPlayers; ++i)
		{
			if( _playerPanels[i]->GetStartPosition() == -1 && 
				_playerPanels[i]->GetSlot() != PlayerPanel::Closed)
			{
				return i;
			}
		}
		return -1;
	}

	void PlayersSettings::AssignColor(int player, int color)
	{
		if(color != -1)
		{
			if(color != 12)
			{
				_colorAssignments[color] = player;
			}
			if( player != -1 && _playerPanels[player]->GetStartPosition() != -1 )
			{
				_parentScreen->GetMapPreview()->SetStartingPositionColor(
					_playerPanels[player]->GetStartPosition(), 
					CreateGameScreen::PlayersColorsList[color]);
			}
		}
		else if( player != -1 && _playerPanels[player]->GetStartPosition() != -1 )
		{
			_parentScreen->GetMapPreview()->SetStartingPositionColor(
				_playerPanels[player]->GetStartPosition(), 
				CreateGameScreen::PlayersColorsList[CreateGameScreen::NoColorIndex]);
		}
		if( player != -1 )
			_playerPanels[player]->SetColor(color);
	}

	void PlayersSettings::DeassignPlayersColor(int player)
	{
		if( player != -1 )
		{
			int color = _playerPanels[player]->GetColor();
			if(color != -1 && color != CreateGameScreen::NoColorIndex)
			{
				_colorAssignments[color] = -1;
			}
			_playerPanels[player]->SetColor(CreateGameScreen::NoColorIndex);
			if(_playerPanels[player]->GetStartPosition() != -1 )
			{
				_parentScreen->GetMapPreview()->SetStartingPositionColor(
					_playerPanels[player]->GetStartPosition(), 
					CreateGameScreen::PlayersColorsList[CreateGameScreen::NoColorIndex]);
			}
		}
	}

	void PlayersSettings::ResetColorAssignments()
	{
		for(unsigned int i = 0; i < _colorAssignments.size(); ++i)
		{
			_colorAssignments[i] = -1;
		}

		for(unsigned int i = 0; i < 8; ++i)
		{
			_parentScreen->GetMapPreview()->SetStartingPositionColor(
				i, CreateGameScreen::PlayersColorsList[CreateGameScreen::NoColorIndex]);
		}
	}

	void PlayersSettings::StartLocChangeRequested(int player, int location)
	{
		int oldLoc = _playerPanels[player]->GetStartPosition();
		// Check if reqested start loc is unassigned
		if( _startLocAssignments[location] == -1 )
		{
			if( oldLoc != - 1)
				DeassignStartLocation(oldLoc);
			AssignStartLocation(player, location);
			return;
		}
		// If is assigned find next unassigned
		int finalLoc = location;
		unsigned int i = location + 1;
		auto& availableStartLocs = _playerPanels[player]->GetAvailableStartPositions();
		for(; i < _startLocAssignments.size(); ++i)
		{
			if( _startLocAssignments[i] == -1 && 
				availableStartLocs.getFirstIndexOf(i) != -1 )
			{
				finalLoc = i;
				break;
			}
		}
		// Reset search if reached end
		if(i == _startLocAssignments.size())
		{
			for(i = 0; i < location; ++i)
			{
				if( _startLocAssignments[i] == -1 && 
					availableStartLocs.getFirstIndexOf(i) != -1 )
				{
					finalLoc = i;
					break;
				}
			}
		}
		if(finalLoc != location)
		{
			// Free location found -> assign it
			if( oldLoc != - 1)
				DeassignStartLocation(oldLoc);
			AssignStartLocation(player, finalLoc);
		}
		// If all are assigned, do nothing
	}

	void PlayersSettings::TeamChangeRequested(int player, int team)
	{
		// Well, teams are only restricted by AvailableTeams, so one passed is always valid (for now)
		_playerPanels[player]->SetTeam(team);
	}

	void PlayersSettings::SlotChangeRequested(int player, int slot)
	{
		if( slot == PlayerPanel::Human )
		{
			// Change to human requested :
			// If human is already assigned switch slots
			if( _humanPlayerAssignment != -1 )
			{
				if( _playerPanels[player]->GetSlot() == PlayerPanel::Closed )
				{
					// Old player with humman slot is to be closed -> deassign its start loc/color
					DeassignStartLocation(player);
					DeassignPlayersColor(_humanPlayerAssignment);
				}
				_playerPanels[_humanPlayerAssignment]->SetSlot(
					_playerPanels[player]->GetSlot());
			}
			_humanPlayerAssignment = player;
		}
		else if ( _humanPlayerAssignment == player )
		{
			// Switched from human to non-human -> reset its assignment
			_humanPlayerAssignment = -1;
		}

		if( slot == PlayerPanel::Closed )
		{
			// Player is to be closed -> deassign its start loc/color
			DeassignStartLocation(player);
			DeassignPlayersColor(player);
		}
		else if( _playerPanels[player]->GetSlot() == PlayerPanel::Closed )
		{
			// Slot is to opened -> request new color
			ColorChangeRequested(player, player);
		}

		_playerPanels[player]->SetSlot(slot);
	}

	void PlayersSettings::FactionChangeRequested(int player, const string& faction)
	{
		// Factions are only restricted by AvailableFactions, so one passed is always valid (for now)
		_playerPanels[player]->SetFaction(faction);
	}

	void PlayersSettings::ColorChangeRequested(int player, int color)
	{
		// Free old color
		DeassignPlayersColor(player);
		// Check if color is in free colors list
		// If not, get 1st next free color
		if( _colorAssignments[color] == -1 )
		{
			AssignColor(player, color);
		}
		else
		{
			// Find next free color
			unsigned int i = color + 1;
			for(; i < _colorAssignments.size(); ++i)
			{
				if( _colorAssignments[i] == -1 )
				{
					AssignColor(player, i);
					break;
				}
			}
			// Reset search if reached end
			if(i == _colorAssignments.size())
			{
				for(i = 0; i < color; ++i)
				{
					if( _colorAssignments[i] == -1 )
					{
						AssignColor(player, i);
						break;
					}
				}
			}
		}
	}

	void PlayersSettings::PlayerIsReadyChanged(int player, bool isReady)
	{
		int openedCount = 0;
		for(unsigned int i = 0; i < _maxPlayers; ++i)
		{
			if(_playerPanels[i]->GetSlot() != PlayerPanel::Closed )
			{
				openedCount += 1;
				if(_playerPanels[i]->GetIsReady() == false)
				{
					if( _allReady == true )
					{
						_allReady = false;
						_allReadyChanged.Fire(false);
					}

					return;
				}
			}
		}
		bool oldReady = _allReady;
		_allReady = openedCount >= 1;
		if( oldReady != _allReady )
		{
			_allReadyChanged.Fire(_allReady);
		}
	}

	int PlayersSettings::GetPlayersStartLocation(int player)
	{
		return _playerPanels[player]->GetStartPosition();
	}

	int PlayersSettings::GetStartLocationsPlayer(int location)
	{
		return location == -1 ? -1 : _startLocAssignments[location];
	}

	void PlayersSettings::ResetMode()
	{
		ResetStartLocationAssignments();
		ResetColorAssignments();
		for(unsigned int i = 0; i < 8; ++i)
		{
			_playerPanels[i]->SetIsAvailable(false);
		}
	}
}