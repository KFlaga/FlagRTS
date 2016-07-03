#pragma once

#include "CommandButtonDefinition.h"

#undef LoadIcon

namespace FlagRTS
{
	// Button that has to be pressed for command to be executed
	// It is shown on unit menu
	class CommandButton
	{
	protected:
		int _hotkey; // Hotkey to be pressed to activate ability 
		uint8 _posX; // X-position of button on unit menu
		uint8 _posY; // Y-position of button on unit menu

		CommandButtonDefinition* _buttonDef;

	public:
		CommandButton(CommandButtonDefinition*);
		~CommandButton();

		void Activate(); // Adds button to unit menu and hotkeys to input contorl
		void Deactivate(); // Removes buttins from unit menu and hotkeys from input control
	
		void LoadIcon();
		void UnloadIcon();

		int GetHotkey() const { return _hotkey; }
		void SetHotkey(int key) { _hotkey = key; }

		uint8 GetPosX() const { return _posX; }
		void SetPosX(int posX) { _posX = posX; }

		uint8 GetPosY() const { return _posY; }
		void SetPosY(int posY) { _posY = posY; }

		const string& GetCommandName() const { return _buttonDef->GetCommandName(); }
		void SetCommandName(const string& name) { _buttonDef->SetCommandName(name); }

		const string& GetCommandDesc() const { return _buttonDef->GetCommandDesc(); }
		void SetCommandDesc(const string& desc) { _buttonDef->SetCommandDesc(desc); }

		const string& GetIconName() const { return _buttonDef->GetIconName(); }
		void SetIconName(const string& icon) { _buttonDef->SetIconName(icon); }

		bool GetShowHotkey() const { return _buttonDef->GetShowHotkey(); }
		void SetShowHotkey(bool value) { _buttonDef->SetShowHotkey(value); }

		bool GetShowEnergy() const { return _buttonDef->GetShowEnergy(); }
		void SetShowEnergy(bool value) { _buttonDef->SetShowEnergy(value); }

		bool GetShowCooldown() const { return _buttonDef->GetShowCooldown(); }
		void SetShowCooldown(bool value) { _buttonDef->SetShowCooldown(value); }

		bool GetShowCost() const { return _buttonDef->GetShowCost(); }
		void SetShowCost(bool value) { _buttonDef->SetShowCost(value); }

		bool GetShowTime() const { return _buttonDef->GetShowTime(); }
		void SetShowTime(bool value) { _buttonDef->SetShowTime(value); }

		bool GetShowProgress() const { return _buttonDef->GetShowProgress(); }
		void SetShowProgress(bool value) { _buttonDef->SetShowProgress(value); }
	};
}