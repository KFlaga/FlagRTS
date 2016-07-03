#include "ResourceStoreSuppliers.h"
#include "ResourceStoreStates.h"
#include "ResourceStoreOrders.h"
#include "ResourceStoreData.h"
#include <Unit.h>
#include <UnitClass.h>

namespace FlagRTS
{
	DllExclusive void ResourceStoreStateSupplier::AddStatesToObject(SceneObject* object)
	{
		Unit* unit = (Unit*)object;
		object->AddState(GetTypeId<StoreDoorOpeningState>(), xNew1(StoreDoorOpeningState,unit));
		object->AddState(GetTypeId<StoreDoorClosingState>(), xNew1(StoreDoorClosingState,unit));
		auto idleState = xNew1(StoreIdleState,unit);
		object->AddState(GetTypeId<StoreIdleState>(), idleState);
		object->AddState(SceneObjectStates::Idle, idleState);
	}

	DllExclusive void ResourceStoreCommandSupplier::AddCommandsToUnit(Unit* unit)
	{
		auto commands = unit->GetCommands();
		StoreCallTransporterCommand* deliverCommand(xNew1(StoreCallTransporterCommand, unit));
		StoreOpenDoorsCommand* openDoors(xNew1(StoreOpenDoorsCommand, unit));
		StoreCloseDoorsCommand* closeDoors(xNew1(StoreCloseDoorsCommand, unit));
		commands->AddCommand(OrderTypes::ReturnResources, deliverCommand, 0);
		commands->AddCommand(GetTypeId<StoreCallTransporterCommand>(), deliverCommand, 0);
		commands->AddCommand(GetTypeId<StoreOpenDoorsCommand>(), openDoors, 0);
		commands->AddCommand(GetTypeId<StoreCloseDoorsCommand>(), closeDoors, 0);
	}

	DllExclusive size_t ResourceStoreKindDataSupplier::GetKindSpecificDataHandle() const
	{
		ResourceStoreTypeData* data = xNew0(ResourceStoreTypeData);
		return reinterpret_cast<size_t>(data);
	}

	DllExclusive void ResourceStoreKindDataSupplier::DeleteHandle(size_t handle) const
	{
		ResourceStoreTypeData* data = reinterpret_cast<ResourceStoreTypeData*>(handle);
		xDelete(data);
	}

	DllExclusive size_t ResourceStoreObjectDataSupplier::GetObjectSpecificDataHandle() const
	{
		ResourceStoreObjectData* data = xNew0(ResourceStoreObjectData);
		return reinterpret_cast<size_t>(data);
	}

	DllExclusive void ResourceStoreObjectDataSupplier::DeleteHandle(size_t handle) const
	{
		ResourceStoreObjectData* data = reinterpret_cast<ResourceStoreObjectData*>(handle);
		xDelete(data);
	}

	using namespace UnitClasses;

	ResourceStoreUnitClassSupplier::ResourceStoreUnitClassSupplier() :
		UnitClassSupplier(
		AddClass(ComplexClasses::Building, SubClasses::ResourceStorage),
			ComplexClasses::None)
	{ }
}