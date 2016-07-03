#include "UnitCommandSet.h"
#include "MainGameObjectPool.h"
#include "DefaultCommands.h"
#include <SystemSettings.h>
#include <KeyBindings.h>
#include "UnitClass.h"
#include "CommandButton.h"
#include "HoverTargetTypes.h"

#undef LoadIcon

namespace FlagRTS
{
	UnitCommandSet::UnitCommandSet() :
		_commands(4),
		_rightCommands(8),
		_leftCommands()
	{

	}

	UnitCommandSet::~UnitCommandSet()
	{
		for(auto commIt = _commands.begin(); commIt != _commands.end(); ++commIt)
		{
			auto val = commIt->Value.first;
			// Look for duplicates
			auto commIt2 = commIt;
			++commIt2;
			for(; commIt2 != _commands.end(); ++commIt2)
			{
				if( commIt2->Value.first == val )
					commIt2->Value.first = 0;
			}
			xDeleteSafe(commIt->Value.first);
		}
	}

	Command* UnitCommandSet::FindCommand(size_t key, CommandTarget* target) 
	{ 
		if(key == OrderTypes::DefaultRight)
		{
			// Find if unit can handle preffered command and target is valid
			Command* command = FindCommand(target->GetPrefferedCommand());
			if(command != 0 && command->IsTargetValid(target) )
			{
				// We can execute this command
				return command;
			}
			// Preffered command failed, so choose default one based on hover target
			auto commIt = _rightCommands.find(target->GetTargetType());
			if( commIt != _rightCommands.end())
				return commIt->Value;

			return _commands[OrderTypes::NoCommand].first;
		}
		else if(key == OrderTypes::DefaultLeft) // Well there should be no left commands
		{
			return _commands[OrderTypes::NoCommand].first;
		}
		else
			return FindCommand(key);
	}

	Command* UnitCommandSet::FindCommandByName(const string& name)
	{
		for(auto commIt = _commands.begin(); commIt != _commands.end(); ++commIt)
		{
			if(name.compare(commIt->Value.first->GetName()) == 0)
			{
				return commIt->Value.first;
			}
		}
		return 0;
	}

	size_t UnitCommandSet::FindCommandKeyByName(const string& name)
	{
		for(auto commIt = _commands.begin(); commIt != _commands.end(); ++commIt)
		{
			if(name.compare(commIt->Value.first->GetName()) == 0)
			{
				return commIt->Key;
			}
		}
		return OrderTypes::NoCommand;
	}

	//UnitCommandSet* UnitCommandSet::GetCopy()
	//{
	//	UnitCommandSet* newSet = xNew0(UnitCommandSet);
	//	// Allocates new commands, and copies their content from other UCS
	//	// Command buttons are copies as pointers
	//	for(auto commIt = _commands.begin(); commIt != _commands.end(); ++commIt)
	//	{
	//		Command* command = commIt->Value.first->GetCopy();
	//		command->SetExecutor(_owner
	//		newSet->AddCommand(commIt->Key, command, commIt->Value.second);
	//		// Copy default mouse commands
	//		// Coping this is a bit trickier - for all commands that uses command from commIt, set newly created command
	//		for(auto rightCommIt = _rightCommands.begin(); rightCommIt != _rightCommands.end(); ++rightCommIt)
	//		{
	//			if(rightCommIt->Value == commIt->Value.first)
	//			{
	//				newSet->AddRightCommand(rightCommIt->Key, command);
	//			}
	//		}

	//		for(auto leftCommIt = _leftCommands.begin(); leftCommIt != _leftCommands.end(); ++leftCommIt)
	//		{
	//			if(leftCommIt->Value == commIt->Value.first)
	//			{
	//				newSet->AddLeftCommand(leftCommIt->Key, command);
	//			}
	//		}
	//	}

	//	return newSet;
	//}
}