#include "TransporterClassSuppliers.h"
#include "TransporterStates.h"
#include "TransporterOrders.h"
#include "TransporterData.h"
#include <Unit.h>
#include <UnitClass.h>

namespace FlagRTS
{
	DllExclusive void TransporterStateSupplier::AddStatesToObject(SceneObject* object)
	{
		Unit* unit = (Unit*)object;
		object->AddState(GetTypeId<TransporterWaitingForMetalMineToBeFreedState>(), xNew1(TransporterWaitingForMetalMineToBeFreedState,unit));
		object->AddState(GetTypeId<TransporterMovingIntoMetalMineDrivewayState>(), xNew1(TransporterMovingIntoMetalMineDrivewayState,unit));
		object->AddState(GetTypeId<TransporterWaitingForMetalMinePackageState>(), xNew1(TransporterWaitingForMetalMinePackageState,unit));
		object->AddState(GetTypeId<TransporterLeavingMetalMineState>(), xNew1(TransporterLeavingMetalMineState,unit));
		object->AddState(GetTypeId<TransporterLeavingMetalMineNoPackageState>(), xNew1(TransporterLeavingMetalMineNoPackageState,unit));
		object->AddState(GetTypeId<TransporterDeliveringPackageState>(), xNew1(TransporterDeliveringPackageState,unit));
		object->AddState(GetTypeId<TransporterWaitingForResourceStoreToBeFreedState>(), xNew1(TransporterWaitingForResourceStoreToBeFreedState,unit));
		object->AddState(GetTypeId<TransporterMovingIntoStoreState>(), xNew1(TransporterMovingIntoStoreState,unit));
		object->AddState(GetTypeId<TransporterInStoreStoreState>(), xNew1(TransporterInStoreStoreState,unit));
		object->AddState(GetTypeId<TransporterMovingOutOfStoreState>(), xNew1(TransporterMovingOutOfStoreState,unit));
		object->AddState(GetTypeId<TransporterPickingUpPackageState>(), xNew1(TransporterPickingUpPackageState,unit));
		
	}

	DllExclusive void TransporterCommandSupplier::AddCommandsToUnit(Unit* unit)
	{
		auto commands = unit->GetCommands();
		DeliverPackageCommand* deliverCommand(xNew1(DeliverPackageCommand, unit));
		PickUpPackageFromMetalMineCommand* pickPackageCommand(xNew1(PickUpPackageFromMetalMineCommand, unit));
		LeaveMetalMineDriveWayCommand* leaveMineCommand(xNew1(LeaveMetalMineDriveWayCommand, unit));
		commands->AddCommand(OrderTypes::ReturnResources, deliverCommand, 0);
		commands->AddCommand(GetTypeId<DeliverPackageCommand>(), deliverCommand, 0);
		commands->AddCommand(OrderTypes::Gather, pickPackageCommand, 0);
		commands->AddCommand(GetTypeId<PickUpPackageFromMetalMineCommand>(), pickPackageCommand, 0);
		commands->AddCommand(GetTypeId<LeaveMetalMineDriveWayCommand>(), leaveMineCommand, 0);
	}

	DllExclusive size_t TransporterKindDataSupplier::GetKindSpecificDataHandle() const
	{
		TransporterTypeData* data = xNew0(TransporterTypeData);
		return reinterpret_cast<size_t>(data);
	}

	DllExclusive void TransporterKindDataSupplier::DeleteHandle(size_t handle) const
	{
		TransporterTypeData* data = reinterpret_cast<TransporterTypeData*>(handle);
		xDelete(data);
	}

	DllExclusive size_t TransporterObjectDataSupplier::GetObjectSpecificDataHandle() const
	{
		TransporterObjectData* data = xNew0(TransporterObjectData);
		return reinterpret_cast<size_t>(data);
	}

	DllExclusive void TransporterObjectDataSupplier::DeleteHandle(size_t handle) const
	{
		TransporterObjectData* data = reinterpret_cast<TransporterObjectData*>(handle);
		xDelete(data);
	}

	using namespace UnitClasses;

	TransporterUnitClassSupplier::TransporterUnitClassSupplier() :
		UnitClassSupplier(
			CreateClass(SubClasses::Worker, SubClasses::TransportResources),
			ComplexClasses::None)
	{ }
}