#pragma once

#include <Array.h>
#include <TypeDefs.h>
#include <GUI.h>
#include <Xml.h>
#include <MyGUI\MyGUI_MouseButton.h>
#include <Event.h>

namespace MyGUI
{
	class Widget;
	class ComboBox;
	class Button;
	class EditBox;
}

namespace FlagRTS
{
	class PlayerPanel
	{
	public:
		// TODO add 8 humans 
		// and some AIs with different difficulty
		enum SlotPlayers
		{
			Closed = 0,
			Human,
			AI
		};

	private:
		MyGUI::Widget* _panel;
		MyGUI::ComboBox* _comboSlot;
		MyGUI::Button* _checkReady;
		MyGUI::Button* _colorBox;
		MyGUI::ComboBox* _comboFaction;
		MyGUI::Button* _buttonTeam;
		MyGUI::EditBox* _labelTeam;
		MyGUI::Button* _buttonStartLoc;
		MyGUI::EditBox* _labelStartLoc;

		int _player;
		int _slot;
		int _color; // Index in PlayersColorsList
		int _faction; // Index in _availableFactions
		int _team; // Index in _availableTeams
		int _startLoc; // Index in _availableStartLocs
		
		bool _isAvailable;
		bool _isCustomisable;
		bool _canBeHuman;
		bool _canBeAi;
		
		Array<int> _availableStartLocs;
		Array<string> _availableFactions;
		Array<int> _availableTeams;
		bool _startLocRestricted;
		bool _factionsRestricted;
		bool _teamsRestricted;

		bool _isReady;

		Event<int, int> _startLocChangeRequested;
		Event<int, int> _teamChangeRequested;
		Event<int, int> _slotChangeRequested;
		Event<int, const string&> _factionChangeRequested;
		Event<int, int> _colorChangeRequested;

		Event<int, bool> _playerIsReadyChanged;

	public:		
		PlayerPanel(RefPtr<GUI> gui, RefPtr<Layout> layout, int player);

		// Accepts <Map> node and reads all player's restrictions for given mode
		// Sets default values for player
		void SetMap(XmlNode* mapNode, XmlNode* modeNode);
	
		int GetPlayer() const { return _player; }
		
		bool GetIsAvailable() const { return _isReady; }
		void SetIsAvailable(bool value);

		int GetSlot() const { return _slot; }
		void SetSlot(int value);

		bool GetIsReady() const { return _isReady; }
		void SetIsReady(bool value);

		int GetColor() const { return _color; }
		void SetColor(int value);

		string GetFaction() const 
		{ 
			return _faction >= 0 ? _availableFactions[_faction] : ""; 
		}
		// Sets player faction, returns false if unavailable faction was choosen
		bool SetFaction(const string& value);
		
		int GetTeam() const 
		{ 
			return _team >= 0 ? _availableTeams[_team] : -1; 
		}
		// Sets player team, returns false if unavailable team was choosen
		bool SetTeam(int value);
		
		int GetStartPosition() const 
		{ 
			return _startLoc >= 0 ? _availableStartLocs[_startLoc] : -1; 
		}
		// Sets player start loc, returns false if unavailable one was choosen
		bool SetStartPosition(int value);

		const Array<int>& GetAvailableStartPositions() const { return _availableStartLocs; }
		const Array<string>& GetAvailableFactions() const { return _availableFactions; }
		const Array<int>& GetAvailableTeams() const { return _availableTeams; }

		void Show();
		void Hide();
		void Resize(float horizontalRatio, float verticalRatio);

		
		// Occurs when StartLocation change button is pressed
		// Start position is not changed for within this class, as
		// it may be occupied already
		// First param is player number, second is requested start location
		Event<int, int>& StartLocChangeRequested() { return _startLocChangeRequested; }
		// Occurs when Team change button is pressed
		// Team is not changed for within this class, as
		// it may be inavailable considering other players settings etc
		// First param is player number, second is requested team
		Event<int, int>& TeamChangeRequested() { return _teamChangeRequested; }
		// Occurs when slot is choosen from combo box
		// It is not changed from within the class, as it may
		// be inavailable etc.
		// First param is player number, second is requested slot
		Event<int, int>& SlotChangeRequested() { return _slotChangeRequested; }
		// Occurs when slot is choosen from combo box
		// It is not changed from within the class, as it may
		// be inavailable etc.
		// First param is player number, second is requested faction
		Event<int, const string&>& FactionChangeRequested() { return _factionChangeRequested; }
		// Occurs when color change button is pressed
		// It is not changed from within the class, as some
		// color way be in use
		// First param is player number, second is requested color index
		Event<int, int>& ColorChangeRequested() { return _colorChangeRequested; }
		// Occurs when ready state is changed
		// First param is player number, second is new readines ( opposite of old one )
		Event<int, bool>& PlayerIsReadyChanged() { return _playerIsReadyChanged; }

	private:
		XmlNode* FindPlayerNode(XmlNode* modeNode);

		void CheckFactionsRestriction(XmlNode* node);
		void CheckStartLocRestriction(XmlNode* node);
		void CheckTeamRestriction(XmlNode* node);
		void ResetRestrictions();
		// Sets new data or disables controls etc, depending on map restrictions
		void UpdateControls();

		void ColorButtonClicked(MyGUI::Widget*, int, int, MyGUI::MouseButton);
		void TeamButtonClicked(MyGUI::Widget*, int, int, MyGUI::MouseButton);
		void StartLocButtonClicked(MyGUI::Widget*, int, int, MyGUI::MouseButton);
		void ReadyButtonClicked(MyGUI::Widget*);
		void SlotSelected(MyGUI::ComboBox* sender, size_t index);
		void FactionSelected(MyGUI::ComboBox* sender, size_t index);
		bool _slotChangeInternal;
		bool _factionChangeInternal;
	};
}