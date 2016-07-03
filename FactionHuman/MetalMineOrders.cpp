#include "MetalMineOrders.h"
#include "MetalMineStates.h"
#include <Unit.h>
#include <UnitClass.h>
#include <DefaultCommandReqs.h>

namespace FlagRTS
{
	DllExclusive MetalMineCallTransporterCommand::MetalMineCallTransporterCommand(Unit* owner) :
		Command(owner, OrderTypes::Gather, CommandCastTypes::SingleTarget, CommandAligments::Neutral)
	{
		_appliance = CommandApplianceTypes::ToSingle;
		_requirements.push_back( xNew3(ExecutorIsConstructedRequirement,
			TechAvailability::Available, TechAvailability::Hidden, this));
	}

	DllExclusive void MetalMineCallTransporterCommand::Update(float ms)
	{

	}

	DllExclusive void MetalMineCallTransporterCommand::Execute()
	{

	}

	DllExclusive void MetalMineCallTransporterCommand::Abort()
	{
		_isFinished = true;
	}

	DllExclusive Command* MetalMineCallTransporterCommand::GetCopy()
	{
		return xNew1(MetalMineCallTransporterCommand, _executor);
	}

	DllExclusive bool MetalMineCallTransporterCommand::IsTargetValid(CommandTarget* target) const
	{
		return UnitClasses::IsUnitOfSubClass(
			target->GetTargetUnit()->GetUnitClass(),
			UnitClasses::TransportResources);
	}

	DllExclusive LoadPackageToTransporterCommand::LoadPackageToTransporterCommand(Unit* owner) :
		Command(owner, OrderTypes::Internal, CommandCastTypes::None, CommandAligments::None)
	{
		_appliance = CommandApplianceTypes::None;
	}

	DllExclusive void LoadPackageToTransporterCommand::Update(float ms)
	{
		if(_isFinished == false)
		{
			if(_executor->GetCurrentState() == GetTypeId<MinePackageReadyState>())
			{
				_executor->ChangeState(GetTypeId<MineLoadingPackageToTransporterState>());
				Abort();
			}
		}
	}

	DllExclusive void LoadPackageToTransporterCommand::Execute()
	{
		// First check if package is ready for loading 
		// so state == MinePackageReadyState
		_isFinished = false;
		// _target.GetTargetUnit()->_onDied += abort()

		if(_executor->GetCurrentState() == GetTypeId<MinePackageReadyState>())
		{
			_executor->ChangeState(GetTypeId<MineLoadingPackageToTransporterState>());
			Abort();
		}
		else if(_executor->GetCurrentState() != GetTypeId<MinePackageReadyState>())
		{
			// If package is not ready or crane is returning with old package
			// Just wait in updates for mine to change state to ready
		}
	}

	DllExclusive void LoadPackageToTransporterCommand::Abort()
	{
		_isFinished = true;
	}

	DllExclusive Command* LoadPackageToTransporterCommand::GetCopy()
	{
		return xNew1(LoadPackageToTransporterCommand, _executor);
	}

	DllExclusive bool LoadPackageToTransporterCommand::IsTargetValid(CommandTarget* target) const
	{
		return true;
	}

	DllExclusive AbandonPackageLoadingCommand::AbandonPackageLoadingCommand(Unit* owner) :
		Command(owner, OrderTypes::Internal, CommandCastTypes::None, CommandAligments::None)
	{
		_appliance = CommandApplianceTypes::None;
	}

	DllExclusive void AbandonPackageLoadingCommand::Update(float ms)
	{

	}

	DllExclusive void AbandonPackageLoadingCommand::Execute()
	{

	}

	DllExclusive void AbandonPackageLoadingCommand::Abort()
	{
		_isFinished = true;
	}

	DllExclusive Command* AbandonPackageLoadingCommand::GetCopy()
	{
		return xNew1(AbandonPackageLoadingCommand, _executor);
	}

	DllExclusive bool AbandonPackageLoadingCommand::IsTargetValid(CommandTarget* target) const
	{
		// It is target less, so return true
		return true;
	}

}