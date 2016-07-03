#include "CommandQueue.h"
#include "UnitCommandSet.h"
#include "DefaultCommands.h"
#include "Unit.h"
#include <Exception.h>

namespace FlagRTS
{
	CommandQueue::CommandQueue(Unit* owner) :
		_owner(owner),
		_currentCommand(0)
	{

	}

	void CommandQueue::ExecuteCommand(int key, CommandTarget* target, bool queue)
	{
		Command* nextCommand = _commands.FindCommand(key, target);
		if(nextCommand == 0)
		{
			// CastException_d("Command not found");
			return;
		}

		nextCommand->CheckRequirements();
		// Dont execute inavailable command
		if(nextCommand->GetAvailability() != TechAvailability::Available)
			return;

		if(_currentCommand == 0) 
		{
			_currentCommand = nextCommand;
			_currentCommand->SetTarget(*target);
			_currentCommand->Execute();
		}
		else if(queue)
		{
			_queue.push_back(nextCommand);
			_queueTargets.push_back(*target);
		}
		else
		{
			_queue.clear();
			_queueTargets.clear();
			_currentCommand->Abort();
			_currentCommand = nextCommand;
			_currentCommand->SetTarget(*target);
			_currentCommand->Execute();
		}
	}
	

	void CommandQueue::ExecuteCommand(Command* command, bool queue)
	{
		if(_currentCommand == 0) 
		{
			_currentCommand = command;
			_currentCommand->SetTarget(*command->GetTargetPtr());
			_currentCommand->Execute();
		}
		else if(queue)
		{
			_queue.push_back(command);
			_queueTargets.push_back(*command->GetTargetPtr());
		}
		else
		{
			_currentCommand->Abort();
			_currentCommand = command;
			_currentCommand->SetTarget(*command->GetTargetPtr());
			_currentCommand->Execute();
		}
	}

	void CommandQueue::Update(float ms)
	{
		if(_currentCommand != 0)
		{
			_currentCommand->Update(ms);

			if( _currentCommand->IsFinished() ||
				_currentCommand->IsSuspended() )
			{
				ExecuteNext();
			}
		}
		else
			ExecuteNext();
	}

	void CommandQueue::ExecuteNext()
	{
		if(_queue.size() > 0)
		{
			_currentCommand = (*_queue.begin());
			_queue.pop_front();
			_currentCommand->SetTarget(*_queueTargets.begin());
			_queueTargets.pop_front();
			if(_currentCommand->IsSuspended())
				_currentCommand->Resume();
			else
				_currentCommand->Execute();
		}
		else
			_currentCommand = 0;
	}

	void CommandQueue::QueuePriorityCommand(int type, CommandTarget* target)
	{
		Command* nextCommand = _commands.FindCommand(type, target);
		if(nextCommand == 0)
		{
			CastException_d("Command not found");
			return;
		}

		if(_currentCommand != 0 &&
			!_currentCommand->IsFinished())
		{
			_queue.push_front(_currentCommand);
			_queueTargets.push_front(*_currentCommand->GetTargetPtr());
			_currentCommand->Suspend();
		}
		_queue.push_front(nextCommand);
		_queueTargets.push_front(*target);
	}

	void CommandQueue::QueuePriorityCommand(Command* command)
	{
		if(_currentCommand != 0 &&
			!_currentCommand->IsFinished())
		{
			_queue.push_front(_currentCommand);
			_queueTargets.push_front(*_currentCommand->GetTargetPtr());
			_currentCommand->Suspend();
		}
		_queue.push_front(command);
		_queueTargets.push_front(*command->GetTargetPtr());
	}

	void CommandQueue::AbortCurrentCommand()
	{
		if(_currentCommand != 0)
		{
			_currentCommand->Abort();
		}
	}
}