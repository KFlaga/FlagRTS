#include "DefaultCommandReqs.h"
#include "Unit.h"
#include "Command.h"

namespace FlagRTS
{
	bool ExecutorIsConstructedRequirement::Condition::CheckCondition()
	{
		_ASSERT(_command != 0);
		return _command->GetExecutor()->IsUnderConstruction() == false;
	}

	bool ExecutorIsImmobilizedRequirement::Condition::CheckCondition()
	{
		_ASSERT(_command != 0);
		return _command->GetExecutor()->IsImmobilized();
	}

	bool ExecutorIsDisarmedRequirement::Condition::CheckCondition()
	{
		_ASSERT(_command != 0);
		return _command->GetExecutor()->IsDisarmed();
	}
	
}