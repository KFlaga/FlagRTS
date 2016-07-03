#include "ResourceStoreOrders.h"
#include "ResourceStoreStates.h"
#include <Unit.h>
#include <UnitClass.h>
#include <DefaultCommandReqs.h>

namespace FlagRTS
{
	DllExclusive StoreCallTransporterCommand::StoreCallTransporterCommand(Unit* owner) :
		Command(owner, OrderTypes::Gather, CommandCastTypes::SingleTarget, CommandAligments::Neutral)
	{
		_appliance = CommandApplianceTypes::ToSingle;
		_requirements.push_back( xNew3(ExecutorIsConstructedRequirement,
			TechAvailability::Available, TechAvailability::Hidden, this));
	}

	DllExclusive void StoreCallTransporterCommand::Update(float ms)
	{

	}

	DllExclusive void StoreCallTransporterCommand::Execute()
	{

	}

	DllExclusive void StoreCallTransporterCommand::Abort()
	{
		_isFinished = true;
	}

	DllExclusive Command* StoreCallTransporterCommand::GetCopy()
	{
		return xNew1(StoreCallTransporterCommand, _executor);
	}

	DllExclusive bool StoreCallTransporterCommand::IsTargetValid(CommandTarget* target) const
	{
		return UnitClasses::IsUnitOfSubClass(
			target->GetTargetUnit()->GetUnitClass(),
			UnitClasses::TransportResources);
	}

	DllExclusive StoreOpenDoorsCommand::StoreOpenDoorsCommand(Unit* owner) :
		Command(owner, OrderTypes::Internal, CommandCastTypes::None, CommandAligments::None)
	{
		_appliance = CommandApplianceTypes::None;
	}

	DllExclusive void StoreOpenDoorsCommand::Update(float ms)
	{
		if(!_isFinished)
		{
			if(_executor->GetCurrentState() == SceneObjectStates::Idle)
			{
				_executor->ChangeState(GetTypeId<StoreDoorOpeningState>());
				Abort();
			}
			else if(_executor->GetCurrentState() == GetTypeId<StoreDoorOpeningState>())
			{
				Abort();
			}
		}
	}

	DllExclusive void StoreOpenDoorsCommand::Execute()
	{
		_isFinished = false;
		// _target.GetTargetUnit()->_onDied += abort()

		if(_executor->GetCurrentState() == SceneObjectStates::Idle)
		{
			_executor->ChangeState(GetTypeId<StoreDoorOpeningState>());
			Abort();
		}
		else if(_executor->GetCurrentState() == GetTypeId<StoreDoorOpeningState>())
		{
			// Store is already opening doors, so do nothing and wait politely
			Abort();
		}
		// Else: store is probably closing its doors, so wait a bit for it
	}

	DllExclusive void StoreOpenDoorsCommand::Abort()
	{
		_isFinished = true;
	}

	DllExclusive Command* StoreOpenDoorsCommand::GetCopy()
	{
		return xNew1(StoreOpenDoorsCommand, _executor);
	}

	DllExclusive bool StoreOpenDoorsCommand::IsTargetValid(CommandTarget* target) const
	{
		return true;
	}

	DllExclusive StoreCloseDoorsCommand::StoreCloseDoorsCommand(Unit* owner) :
		Command(owner, OrderTypes::Internal, CommandCastTypes::None, CommandAligments::None)
	{
		_appliance = CommandApplianceTypes::None;
	}

	DllExclusive void StoreCloseDoorsCommand::Update(float ms)
	{
		if(!_isFinished)
		{
			if(_executor->GetCurrentState() == SceneObjectStates::Idle)
			{
				_executor->ChangeState(GetTypeId<StoreDoorClosingState>());
				Abort();
			}
			else if(_executor->GetCurrentState() == GetTypeId<StoreDoorClosingState>())
			{
				Abort();
			}
		}
	}

	DllExclusive void StoreCloseDoorsCommand::Execute()
	{
		_isFinished = false;
		// _target.GetTargetUnit()->_onDied += abort()

		if(_executor->GetCurrentState() == SceneObjectStates::Idle)
		{
			_executor->ChangeState(GetTypeId<StoreDoorClosingState>());
			Abort();
		}
		else if(_executor->GetCurrentState() == GetTypeId<StoreDoorClosingState>())
		{
			// Store is already opening doors, so do nothing and wait politely
			Abort();
		}
		// Else: store is probably closing its doors, so wait a bit for it
	}

	DllExclusive void StoreCloseDoorsCommand::Abort()
	{
		_isFinished = true;
	}

	DllExclusive Command* StoreCloseDoorsCommand::GetCopy()
	{
		return xNew1(StoreCloseDoorsCommand, _executor);
	}

	DllExclusive bool StoreCloseDoorsCommand::IsTargetValid(CommandTarget* target) const
	{
		// It is target less, so return true
		return true;
	}
}