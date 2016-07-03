#pragma once

#include "UnitCommandSet.h"
#include "Command.h"
#include <List.h>

namespace FlagRTS
{
	class Unit;

	class CommandQueue
	{
	private:
		Unit* _owner;
		UnitCommandSet _commands;
		List<Command*> _queue;
		List<CommandTarget> _queueTargets;
		Command* _currentCommand;

	public:
		CommandQueue(Unit* owner);

		// Finds command of given type and executes it or queue its execution
		void ExecuteCommand(int type, CommandTarget* target, bool queue = false);
		// Executes given command or queue its execution
		void ExecuteCommand(Command* command, bool queue = false);

		// Updates command and if command is finished executes next queued command
		void Update(float ms);

		// Gets currently executing command
		Command* GetCurrentCommand() const { return _currentCommand; }
		// Finds unit's command of given type for given target
		Command* FindCommand(int type, CommandTarget* target)
		{
			return _commands.FindCommand(type, target);
		}

		// Adds command to begining of command queue, suspends current command execution
		// and starts this command execution. Currently executed command is resumed
		// after this finishes
		void QueuePriorityCommand(int type, CommandTarget* target);
		void QueuePriorityCommand(Command* command);

		// Forces end of current command execution ( new one starts with next update
		// if one is queued ) and set sets current commands as 0
		void AbortCurrentCommand();

		UnitCommandSet& GetCommands() { return _commands; }

	private:
		void ExecuteNext();
	};
}