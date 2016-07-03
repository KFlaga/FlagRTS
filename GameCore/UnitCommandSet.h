#pragma once

#include <Xml.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	class Command;
	class CommandButton;
	class CommandTarget;

	// Defines all commands/abilities available to specific unit as well as
	// hotkeys and buttons to use for commands/abilities
	// 
	class UnitCommandSet
	{
	private:
		// Binds commands with buttons to command type for non-ability commands
		// for ability commands binds to hotkey
		typedef ArrayMap<size_t, std::pair<Command*, CommandButton*>> CommandMap;
		CommandMap _commands; 
		// Binds commands to target types for default right command
		// Command pointers are the same as used in CommandMap, so ones from this map
		// should not be deleted
		ArrayMap<size_t, Command*> _rightCommands;
		// Binds commands to target types for default left command
		// Command pointers are the same as used in CommandMap, so ones from this map
		// should not be deleted
		ArrayMap<size_t, Command*> _leftCommands;

	public:
		UnitCommandSet();
		~UnitCommandSet();

		// Find command with given command type and if its right/left command
		// then with given command target
		Command* FindCommand(size_t key, CommandTarget* target);

		// Returns command with given name
		Command* FindCommandByName(const string& name);
		// Returns command's key with given name
		size_t FindCommandKeyByName(const string& name);

		// Find command with given command type ( do not handle right/left commands )
		Command* FindCommand(size_t key) 
		{ 
			auto commIt = _commands.find(key);
			if(commIt != _commands.end())
			{
				return commIt->Value.first;
			}
			else
				return 0;
		}

		void AddCommand(size_t key, Command* command, CommandButton* button) 
		{ 
			_commands.insert(std::make_pair(key,std::make_pair(command, button))); 
		}

		void AddRightCommand(size_t key, Command* command) 
		{ 
			_rightCommands[key] = command; 
		}

		void AddLeftCommand(size_t key, Command* command) 
		{ 
			_leftCommands[key] = command; 
		}
	
		void SetCommandButton(size_t key, CommandButton* button)
		{
			_commands.find(key)->Value.second = button;
		}

		// Allocates and returns copy of this command set
		//UnitCommandSet* GetCopy();

		CommandMap::iterator GetCommandsBegin() { return _commands.begin(); }
		CommandMap::iterator GetCommandsEnd() { return _commands.end(); }

	private:
		int ParseCommandType(const string& type);
	};
}