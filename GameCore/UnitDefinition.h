#pragma once

#include "PhysicalObjectDefinition.h"
#include "UnitProperties.h"
#include "WeaponDefinition.h"
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

	struct TrainableUnit
	{
		const char* DefinitionName;
		int Number;
		Array<const char*> RequirementNames;
	};

	class UnitDefinition : public PhysicalObjectDefinition
	{
	protected:
		string _iconName;
		string _guiName; // Unit name in unit menu

		UnitStats* _baseStats;

		// List of all unit's weapons and bool value indicating if its active or not
		typedef Array<std::pair<WeaponDefinition*, bool>> WeaponList;
		WeaponList _weapons;

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
		// Event occurs when unit is selected/deselected ( param is true if selected )
		Event<Unit*, bool> _onIsSelectedChanged;
		SceneMarkerDefinition* _selectionMarkerDef;
		Vector3 _selectionMarkerSize;
		Vector3 _selectionMarkerOffset;

		// If true unit can train units and have a TrainingQueue
		bool _canTrainUnits;
		// Array of unit def names and their numbers ( unit with number x will be bind to Train{x} command )
		// When creating unit, commands for training units form this array will be added
		typedef Array<TrainableUnit*> TrainableUnitsArray;
		TrainableUnitsArray _trainableUnitsDefs;
	
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

		void SetWeapons(const WeaponList& value) { _weapons = value; }
		const WeaponList& GetWeapons() const { return _weapons; }
		WeaponList& GetWeapons() { return _weapons; }

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

		void SetCanTrainUnits(bool value) { _canTrainUnits = value; }
		bool CanTrainUnits() const { return _canTrainUnits; }

		const TrainableUnitsArray& GetTrainableUnitsArray() const { return _trainableUnitsDefs; }
		TrainableUnitsArray& GetTrainableUnitsArray() { return _trainableUnitsDefs; }

		Event<Unit*, Unit*, bool>& GetOnConstructionFinishedEvent() { return _onConstructionFinsished; }

		Event<Unit*, bool>& GetOnIsSelectedChangedEvent() { return _onIsSelectedChanged; }

		Array<Media::SoundSource*>& GetUnitSoundSet() { return _unitSoundSet; }
		const Array<Media::SoundSource*>& GetUnitSoundSet() const { return _unitSoundSet; }

		bool CountsAsPlayerUnit() const { return _countsAsPlayerUnit; }
		void SetCountsAsPlayerUnit(bool value) { _countsAsPlayerUnit = value; }

		SceneMarkerDefinition* GetSelectionMarkerDefinition() const { return _selectionMarkerDef; }
		void SetSelectionMarkerDefinition(SceneMarkerDefinition* markerDef) { _selectionMarkerDef = markerDef; }
		
		const Vector3& GetSelectionMarkerSize() const { return _selectionMarkerSize; }
		void SetSelectionMarkerSize(const Vector3& value) { _selectionMarkerSize = value; }
		
		const Vector3& GetSelectionMarkerOffset() const { return _selectionMarkerOffset; }
		void SetSelectionMarkerOffset(const Vector3& value) { _selectionMarkerOffset = value; }

	private:
		void ParseUnitProperties(XmlNode* propsNode);
		void ParseUnitSounds(XmlNode* soundsNode);
		void ParseCommands(XmlNode* commsNode);
		void ParseWeapons(XmlNode* weaponsNode);
		void ParseTrainableUnits(XmlNode* trainableNode);
		void ParseSelectionMarker(XmlNode* selectionNode);
		void ParseMoveStrategy(XmlNode* moveNode);
	};
}