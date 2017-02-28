#pragma once

#include "PhysicalObjectDefinition.h"
#include "UnitProperties.h"
#include <List.h>

namespace FlagRTS
{
	class UnitCommandSet;
	struct UnitStats;
	class CommandButton;
	class Unit;
	class Command;
	class SceneMarkerDefinition;
	class IMoveStrategy;

	namespace Media
	{
		class SoundSource;
	}

	class UnitDefinition : public PhysicalObjectDefinition
	{
	protected:
		string _iconName;
		string _guiName; // Unit name in unit menu

		UnitStats* _baseStats;

		// Command buttons for default command
		typedef Array<std::pair<int,CommandButton*>> CommandButtonsArray;
		CommandButtonsArray _defaultCommandButtons;
		// Commands + buttons for custom commands
		typedef Array<std::pair<Command*,CommandButton*>> CustomCommandsArray;
		CustomCommandsArray _customCommands;

		IMoveStrategy* _defaultMoveStrategy;

		// Standard sounds used by this unit. May be enlarged by additional sounds played on events
		Array<Media::SoundSource*> _unitSoundSet;

		// Event fired when unit is constructed (buildings) / trained (mobiles)
		// Fist arg is builder, second is finished unit
		Event<Unit*, Unit*, bool> _onConstructionFinsished;
	
		bool _countsAsPlayerUnit; // If false, player is not considered to be owning this unit -> must be false for
								  // all neutral units + maybe for integral parts of other units ( CounstructionSite, AutoTurrets etc )
								  // (?? where have I used it ? )
	public:
		UnitDefinition();
		UnitDefinition(XmlNode* unitNode);
		~UnitDefinition();

		size_t GetUnitKind() const { return GetHandle().Object; }

		void SetStats(UnitStats* value) { _baseStats = value; }
		const UnitStats* GetStats() const { return _baseStats; }
		UnitStats* GetStats() { return _baseStats; }

		void SetIcon(const string& icon) { _iconName = icon; }
		const string& GetIcon() const { return _iconName; }

		void SetGuiName(const string& name) { _guiName = name; }
		const string& GetGuiName() const { return _guiName; }

		void SetDefaultCommandButtons(const CommandButtonsArray& value) { _defaultCommandButtons = value; }
		const CommandButtonsArray& GetDefaultCommandButtons() const { return _defaultCommandButtons; }
		CommandButtonsArray& GetDefaultCommandButtons() { return _defaultCommandButtons; }

		void SetCustomCommands(const CustomCommandsArray& value) { _customCommands = value; }
		const CustomCommandsArray& GetCustomCommands() const { return _customCommands; }
		CustomCommandsArray& GetCustomCommands() { return _customCommands; }

		IMoveStrategy* GetMoveStrategy() const { return _defaultMoveStrategy; }
		void SetMoveStrategy(IMoveStrategy* value) { _defaultMoveStrategy = value; }

		Event<Unit*, Unit*, bool>& GetOnConstructionFinishedEvent() { return _onConstructionFinsished; }

		Array<Media::SoundSource*>& GetUnitSoundSet() { return _unitSoundSet; }
		const Array<Media::SoundSource*>& GetUnitSoundSet() const { return _unitSoundSet; }

		bool CountsAsPlayerUnit() const { return _countsAsPlayerUnit; }
		void SetCountsAsPlayerUnit(bool value) { _countsAsPlayerUnit = value; }

	private:
		void ParseUnitProperties(XmlNode* propsNode);
		void ParseUnitSounds(XmlNode* soundsNode);
		void ParseCommands(XmlNode* commsNode);
		void ParseMoveStrategy(XmlNode* moveNode);
	};
}