#include "FactoryOrders.h"
#include "FactoryStates.h"
#include <Unit.h>
#include <UnitClass.h>
#include <DefaultCommandReqs.h>

namespace FlagRTS
{
	DllExclusive FactoryOpenDoorsCommand::FactoryOpenDoorsCommand(Unit* owner) :
		Command(owner, OrderTypes::Internal, CommandCastTypes::None, CommandAligments::None)
	{
		_appliance = CommandApplianceTypes::None;
	}

	DllExclusive void FactoryOpenDoorsCommand::Update(float ms)
	{
		if(!_isFinished)
		{
			if(_executor->GetCurrentState() == SceneObjectStates::Idle)
			{
				_executor->ChangeState(GetTypeId<FactoryDoorOpeningState>());
				Abort();
			}
			else if(_executor->GetCurrentState() == GetTypeId<FactoryDoorOpeningState>())
			{
				Abort();
			}
		}
	}

	DllExclusive void FactoryOpenDoorsCommand::Execute()
	{
		_isFinished = false;

		if(_executor->GetCurrentState()  == GetTypeId<FactoryDoorOpeningState>())
		{
			// Store is already opening doors, so do nothing and wait politely
			Abort();
		}
		else if(_executor->GetCurrentState())
		{
			_executor->ChangeState(GetTypeId<FactoryDoorOpeningState>());
			Abort();
		}
	}

	DllExclusive void FactoryOpenDoorsCommand::Abort()
	{
		_isFinished = true;
	}

	DllExclusive Command* FactoryOpenDoorsCommand::GetCopy()
	{
		return xNew1(FactoryOpenDoorsCommand, _executor);
	}

	DllExclusive bool FactoryOpenDoorsCommand::IsTargetValid(CommandTarget* target) const
	{
		return true;
	}

	DllExclusive FactoryCloseDoorsCommand::FactoryCloseDoorsCommand(Unit* owner) :
		Command(owner, OrderTypes::Internal, CommandCastTypes::None, CommandAligments::None)
	{
		_appliance = CommandApplianceTypes::None;
	}

	DllExclusive void FactoryCloseDoorsCommand::Update(float ms)
	{
		if(!_isFinished)
		{
			if(_executor->GetCurrentState() == SceneObjectStates::Idle)
			{
				_executor->ChangeState(GetTypeId<FactoryDoorClosingState>());
				Abort();
			}
			else if(_executor->GetCurrentState() == GetTypeId<FactoryDoorClosingState>())
			{
				Abort();
			}
		}
	}

	DllExclusive void FactoryCloseDoorsCommand::Execute()
	{
		_isFinished = false;

		if(_executor->GetCurrentState()  == GetTypeId<FactoryDoorClosingState>())
		{
			// Store is already opening doors, so do nothing and wait politely
			Abort();
		}
		else if(_executor->GetCurrentState())
		{
			_executor->ChangeState(GetTypeId<FactoryDoorClosingState>());
			Abort();
		}
	}

	DllExclusive void FactoryCloseDoorsCommand::Abort()
	{
		_isFinished = true;
	}

	DllExclusive Command* FactoryCloseDoorsCommand::GetCopy()
	{
		return xNew1(FactoryCloseDoorsCommand, _executor);
	}

	DllExclusive bool FactoryCloseDoorsCommand::IsTargetValid(CommandTarget* target) const
	{
		// It is target less, so return true
		return true;
	}
}