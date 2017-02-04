#pragma once

#include "Technology.h"
#include "TechRequirement.h"

namespace FlagRTS
{
	class Command;

	class ICommandCondition : public IParametrizedCondition<uint8>
	{
	protected:
		Command* _command;

	public:
		ICommandCondition() :
			_command(0)
		{ }

		ICommandCondition(Command* command) :
			_command(command)
		{ }

		void SetCommand(Command* command) { _command = command; }
		Command* GetCommand() const { return _command; }
	};

	// Requirements for commands -> extends basics requirements with usage of owning command
	// or rather its executor and executor's state 
	// Indended to make available commands dependend on executor state ( like disables / buffs /
	// being under construction / battle stances ( artillery deployment ) )
	class CommandRequirement : public TechRequirement
	{
	protected:
		Command* _command;

	public:
		CommandRequirement(TechAvailability onTrue, TechAvailability onFalse, ICommandCondition* condition) :
			TechRequirement(onTrue, onFalse, condition),
			_command(0)
		{ 
			//SetFinalType(CommandState);
		}

		CommandRequirement(TechAvailability onTrue, TechAvailability onFalse, ICommandCondition* condition, Command* command) :
			TechRequirement(onTrue, onFalse, condition),
			_command(command)
		{ 
			//SetHandleTypePart(CommandState);
		}

		~CommandRequirement()
		{
			
		}

		void SetCommand(Command* command)
		{
			static_cast<ICommandCondition*>(_condition)->SetCommand(command);
		}

		Command* GetCommand() const { return _command; }
	};
}