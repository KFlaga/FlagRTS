#include "TransporterOrders.h"
#include <Unit.h>
#include <UnitClass.h>
#include "TransporterData.h"
#include "TransporterStates.h"
#include "MetalMineData.h"
#include "ResourceStoreData.h"
#include <HoverTargetTypes.h>

#include <GameWorld.h>
#include <PathingSystem.h>
#include <DefaultCommandReqs.h>

namespace FlagRTS
{
	DllExclusive DeliverPackageCommand::DeliverPackageCommand(Unit* owner) :
		Command(owner, OrderTypes::ReturnResources, CommandCastTypes::SingleTarget, CommandAligments::Neutral)
	{
		_appliance = CommandApplianceTypes::ToKind;
	}

	DllExclusive void DeliverPackageCommand::Update(float ms)
	{
		if(_isFinished == false)
		{
			// Transporter is currently moving towards store
			// Check if is near the spot
			if( _executor->GetPositionAbsolute().squaredDistance(_storeSpot) < 1500.f )
			{
				// Transporter is near the spot -> may change it state to wait
				// for free slot in store
				GameWorld::GlobalWorld->GetPathingSystem()->AbandonPath(_executor);
				_executor->ChangeState(GetTypeId<TransporterWaitingForResourceStoreToBeFreedState>());
				_isFinished = true;
			}
		}
	}

	DllExclusive void DeliverPackageCommand::Execute()
	{
		_isFinished = false;
		
		// Save store in transporter data
		TransporterObjectData* transporterData = _executor->
			GetObjectSpecificData<TransporterObjectData>();
		transporterData->TargetStore = _target.GetTargetUnit();

		if( transporterData->Package == 0)
		{
			// We dont have package, so treat is as default move order
			Abort();
			_target.SetTargetType(HoverTargetType::OwnUnit);
			_executor->GetCommandQueue()->ExecuteCommand(
				OrderTypes::Move, &_target);
			return;
		}

		// Check if we're close to store spot
		ResourceStoreTypeData* storeTypeData = _target.GetTargetUnit()->
			GetKindSpecificData<ResourceStoreTypeData>();

		_storeSpot = _target.GetTargetUnit()->GetPositionAbsolute() + 
			_target.GetTargetUnit()->GetOrientation() * storeTypeData->DoorSpot;

		CommandTarget ct;
		ct.SetTargetUnit(0);
		ct.SetTargetType(HoverTargetType::Terrain);
		ct.SetPointTarget(_storeSpot);

		_executor->GetCommandQueue()->QueuePriorityCommand(
			OrderTypes::Move, &ct);
	}

	DllExclusive void DeliverPackageCommand::Abort()
	{
		_isFinished = true;
	}
	
	DllExclusive void DeliverPackageCommand::Suspend()
	{
		_isSuspended = true;
	}

	DllExclusive void DeliverPackageCommand::Resume()
	{
		_isSuspended = false;
	}

	DllExclusive Command* DeliverPackageCommand::GetCopy()
	{
		return xNew1(DeliverPackageCommand, _executor);
	}

	DllExclusive bool DeliverPackageCommand::IsTargetValid(CommandTarget* target) const
	{
		// Check if target is unit and is friendly store
		return (target->GetTargetUnit() != 0) && 
			UnitClasses::IsUnitOfSubClass(
			target->GetTargetUnit()->GetUnitClass(),
			UnitClasses::ResourceStorage);
	}

	DllExclusive PickUpPackageFromMetalMineCommand::PickUpPackageFromMetalMineCommand(Unit* owner) :
		Command(owner, OrderTypes::Gather, CommandCastTypes::SingleTarget, CommandAligments::Neutral)
	{
		_appliance = CommandApplianceTypes::ToKind;
	}

	DllExclusive void PickUpPackageFromMetalMineCommand::Update(float ms)
	{
		if(_isFinished == false)
		{
			// Transporter is currently moving towards mine
			// Check if is near the spot
			if( _executor->GetPositionAbsolute().squaredDistance(_mineDriveInSpot) < 1500.f )
			{
				// Transporter is near the spot -> may change it state to wait
				// for free spot on mine
				GameWorld::GlobalWorld->GetPathingSystem()->AbandonPath(_executor);
				_executor->ChangeState(GetTypeId<TransporterWaitingForMetalMineToBeFreedState>());
				_isFinished = true;
			}
		}
	}

	DllExclusive void PickUpPackageFromMetalMineCommand::Execute()
	{
		// 1) Set correct mine for transporter
		TransporterObjectData* transporterData = _executor->GetObjectSpecificData<TransporterObjectData>();
		transporterData->TargetMine = _target.GetTargetUnit(); // We know its mine from validity check
		_isFinished = false;

		// 2) Check if transporter have a package already
		if(transporterData->Package != 0 == true)
		{
			// Deliver it first
			Abort();
			_executor->ChangeState( GetTypeId<TransporterDeliveringPackageState>() );
			return;
		}

		auto mineObjectData = transporterData->TargetMine->
			GetObjectSpecificData<MetalMineObjectData>();
		// 3) Check if mine is not depleted
		if(mineObjectData->AvailableMetal <= 0 && mineObjectData->PackageReady == false)
		{
			Abort();
			return;
		}

		Vector3 finalSpot = transporterData->TargetMine->
			GetKindSpecificData<MetalMineTypeData>()->TransporterDriveInSpot;

		// 4) Everything is ok, so queue move command
		_mineDriveInSpot = _target.GetTargetUnit()->GetPositionAbsolute() + 
			_target.GetTargetUnit()->GetOrientation() * finalSpot;

		CommandTarget ct;
		ct.SetTargetUnit(0);
		ct.SetTargetType(HoverTargetType::Terrain);
		ct.SetPointTarget(_mineDriveInSpot);

		_executor->GetCommandQueue()->QueuePriorityCommand(
			OrderTypes::Move, &ct);

		// Command will be suspended after this call and will resume
		// move order is issued
	}

	DllExclusive void PickUpPackageFromMetalMineCommand::Abort()
	{
		Command::Abort();
	}

	DllExclusive void PickUpPackageFromMetalMineCommand::Suspend()
	{
		_isSuspended = true;
	}

	DllExclusive void PickUpPackageFromMetalMineCommand::Resume()
	{
		_isSuspended = false;
	}

	DllExclusive Command* PickUpPackageFromMetalMineCommand::GetCopy()
	{
		return xNew1(PickUpPackageFromMetalMineCommand, _executor);
	}

	DllExclusive bool PickUpPackageFromMetalMineCommand::IsTargetValid(CommandTarget* target) const
	{
		return (target->GetTargetUnit() != 0) &&
			UnitClasses::IsUnitOfComplexClass(
			target->GetTargetUnit()->GetUnitClass(),
			UnitClasses::ResourceMine);
	}

	// Orders transporter to leave mine's driveway
	// Not casted directly, but only if transporter is forces to leave by another commands/objects
	// It is used for normal ( package is loaded ) and anormal circumstances ( like forced transporter movement )
	// Also executes Mine's AbanodonLoad command ( which will do nothing if package is loaded successfully )
	DllExclusive LeaveMetalMineDriveWayCommand::LeaveMetalMineDriveWayCommand(Unit* owner) :
		Command(owner, OrderTypes::Internal, CommandCastTypes::None, CommandAligments::Neutral)
	{
		_appliance = CommandApplianceTypes::None;
	}

	DllExclusive void LeaveMetalMineDriveWayCommand::Update(float ms)
	{

	}

	DllExclusive void LeaveMetalMineDriveWayCommand::Execute()
	{
		TransporterObjectData* transporterData = _executor->GetObjectSpecificData<TransporterObjectData>();
		transporterData->TargetMine = _target.GetTargetUnit(); // We know its mine from validity check
		_isFinished = false;

		// 2) If transporter have a package, then just leave the mine
		if(transporterData->Package != 0)
		{
			_executor->ChangeState(GetTypeId<TransporterLeavingMetalMineState>());
			Abort();
		}
		else
		{
			// If it dont have it ( i.e. mine was destroyed ) change to another state
			_executor->ChangeState(GetTypeId<TransporterLeavingMetalMineNoPackageState>());
			Abort();
		}
	}

	DllExclusive void LeaveMetalMineDriveWayCommand::Abort()
	{
		_isFinished = true;
	}

	DllExclusive Command* LeaveMetalMineDriveWayCommand::GetCopy()
	{
		return xNew1(LeaveMetalMineDriveWayCommand, _executor);
	}

	DllExclusive bool LeaveMetalMineDriveWayCommand::IsTargetValid(CommandTarget* target) const
	{
		return true;
	}
}