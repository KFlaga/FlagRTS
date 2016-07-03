#pragma once

#include <Event.h>
#include <Xml.h>
#include <Array.h>
#include <GUI.h>

namespace MyGUI
{
	class Widget;
}

namespace FlagRTS
{
	class PlayerPanel;
	class CreateGameScreen;

	class PlayersSettings
	{
	private:
		CreateGameScreen* _parentScreen;
		Array<PlayerPanel*> _playerPanels;
		MyGUI::Widget* _panelPlayers;

		int _maxPlayers;
		int _startLocCount;
		Array<int> _startLocAssignments; // Map : index = start loc, value = player assigned or -1
		Array<int> _colorAssignments; // Map : index = color, value = player assigned or -1
		int _humanPlayerAssignment; // Player number of human player

		bool _allReady;
		Event<bool> _allReadyChanged;

	public:
		PlayersSettings(CreateGameScreen* parent, 
			RefPtr<GUI> gui, RefPtr<Layout> layout);
		~PlayersSettings();

		void SetMode(XmlNode* mapNode, const string& modeName);
		void ResetMode();

		int GetPlayersStartLocation(int player);
		int GetStartLocationsPlayer(int location);

		// Returns next player that have no start location assigned
		int GetNextNotAssignedPlayer();
		// Unassigns all start locations
		void ResetStartLocationAssignments();
		void AssignStartLocation(int player, int location);
		void DeassignStartLocation(int location);
		// Assigns next unassigned player to given start location
		void AssignNextPlayerToStartLocation(int location);
		void SwitchStartLocationAssignments(int loc1, int loc2);

		void ResetColorAssignments();
		void AssignColor(int player, int color);
		void DeassignPlayersColor(int player);

		void Show();
		void Hide();
		void Resize(float hRatio, float vRatio);

		bool GetAllReady() const { return _allReady; }

		const Array<PlayerPanel*>& GetPlayerPanels() { return _playerPanels; }

		// Occurs when all players are changed to ready or some become not ready
		// Parameter indicates if all players are ready or not
		Event<bool>& GetAllReadyChangedEvent() { return _allReadyChanged; }

	private:
		XmlNode* FindModeNode(XmlNode* mapNode, const string& modeName);

		void StartLocChangeRequested(int player, int location);
		DEFINE_DELEGATE2(StartLocChangeRequestedDelegate, 
			PlayersSettings,
			StartLocChangeRequested, int, int);
		StartLocChangeRequestedDelegate _onStartLocChangeRequested;
		
		void TeamChangeRequested(int player, int team);
		DEFINE_DELEGATE2(TeamChangeRequestedDelegate, 
			PlayersSettings,
			TeamChangeRequested, int, int);
		TeamChangeRequestedDelegate _onTeamChangeRequested;
		
		void SlotChangeRequested(int player, int slot);
		DEFINE_DELEGATE2(SlotChangeRequestedDelegate, 
			PlayersSettings,
			SlotChangeRequested, int, int);
		SlotChangeRequestedDelegate _onSlotChangeRequested;
		
		void FactionChangeRequested(int player, const string& faction);
		DEFINE_DELEGATE2(FactionChangeRequestedDelegate, 
			PlayersSettings,
			FactionChangeRequested, int, const string&);
		FactionChangeRequestedDelegate _onFactionChangeRequested;
		
		void ColorChangeRequested(int player, int color);
		DEFINE_DELEGATE2(ColorChangeRequestedDelegate, 
			PlayersSettings,
			ColorChangeRequested, int, int);
		ColorChangeRequestedDelegate _onColorChangeRequested;

		void PlayerIsReadyChanged(int player, bool isReady);
		DEFINE_DELEGATE2(PlayerIsReadyChangedDelegate, 
			PlayersSettings,
			PlayerIsReadyChanged, int, bool);
		PlayerIsReadyChangedDelegate _onPlayerIsReadyChanged;
	};
}