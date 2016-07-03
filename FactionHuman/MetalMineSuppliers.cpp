#include "MetalMineClassSuppliers.h"
#include "MetalMineStates.h"
#include "MetalMineOrders.h"
#include "MetalMineData.h"
#include <Unit.h>
#include <UnitClass.h>

namespace FlagRTS
{
	DllExclusive void MetalMineStateSupplier::AddStatesToObject(SceneObject* object)
	{
		Unit* unit = (Unit*)object;
		object->AddState(GetTypeId<MinePackageReadyState>(), xNew1(MinePackageReadyState,unit));
		object->AddState(GetTypeId<MineLoadingPackageToTransporterState>(), xNew1(MineLoadingPackageToTransporterState,unit));
		object->AddState(GetTypeId<MineReturningWithPackageState>(), xNew1(MineReturningWithPackageState,unit));
		auto waitingState = xNew1(MineWaitingForPackageState,unit);
		object->AddState(GetTypeId<MineWaitingForPackageState>(), waitingState);
		object->AddState(SceneObjectStates::Idle, waitingState);
	}

	DllExclusive void MetalMineCommandSupplier::AddCommandsToUnit(Unit* unit)
	{
		auto commands = unit->GetCommands();
		MetalMineCallTransporterCommand* callTransporterCommand(xNew1(MetalMineCallTransporterCommand, unit));
		LoadPackageToTransporterCommand* loadPackageCommand(xNew1(LoadPackageToTransporterCommand, unit));
		AbandonPackageLoadingCommand* abandonLoadingCommand(xNew1(AbandonPackageLoadingCommand, unit));
		commands->AddCommand(OrderTypes::Gather, callTransporterCommand, 0);
		commands->AddCommand(GetTypeId<LoadPackageToTransporterCommand>(), loadPackageCommand, 0);
		commands->AddCommand(GetTypeId<AbandonPackageLoadingCommand>(), abandonLoadingCommand, 0);
	}

	DllExclusive size_t MetalMineKindDataSupplier::GetKindSpecificDataHandle() const
	{
		MetalMineTypeData* data = xNew0(MetalMineTypeData);
		return reinterpret_cast<size_t>(data);
	}

	DllExclusive void MetalMineKindDataSupplier::DeleteHandle(size_t handle) const
	{
		MetalMineTypeData* data = reinterpret_cast<MetalMineTypeData*>(handle);
		xDelete(data);
	}

	DllExclusive size_t MetalMineObjectDataSupplier::GetObjectSpecificDataHandle() const
	{
		MetalMineObjectData* data = xNew0(MetalMineObjectData);
		return reinterpret_cast<size_t>(data);
	}

	DllExclusive void MetalMineObjectDataSupplier::DeleteHandle(size_t handle) const
	{
		MetalMineObjectData* data = reinterpret_cast<MetalMineObjectData*>(handle);
		xDelete(data);
	}

	using namespace UnitClasses;

	MetalMineUnitClassSupplier::MetalMineUnitClassSupplier() :
		UnitClassSupplier(
			ComplexClasses::ResourceMine,
			ComplexClasses::None)
	{ }
}