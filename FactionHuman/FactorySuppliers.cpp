#include "FactoryClassSuppliers.h"
#include "FactoryStates.h"
#include "FactoryOrders.h"
#include "FactoryData.h"
#include <Unit.h>
#include <UnitClass.h>

namespace FlagRTS
{
	DllExclusive void FactoryStateSupplier::AddStatesToObject(SceneObject* object)
	{
		Unit* unit = (Unit*)object;
		object->AddState(GetTypeId<FactoryDoorOpeningState>(), xNew1(FactoryDoorOpeningState,unit));
		object->AddState(GetTypeId<FactoryDoorClosingState>(), xNew1(FactoryDoorClosingState,unit));
		auto trainingState = xNew1(FactoryTrainingState,unit);
		object->AddState(GetTypeId<FactoryTrainingState>(), trainingState);
		object->AddState(SceneObjectStates::Train, trainingState);
		auto unitMovingOutState = xNew1(FactoryUnitMovingOutState, unit);
		object->AddState(GetTypeId<FactoryUnitMovingOutState>(), unitMovingOutState);
	}

	DllExclusive void FactoryCommandSupplier::AddCommandsToUnit(Unit* unit)
	{
		auto commands = unit->GetCommands();
		FactoryOpenDoorsCommand* openDoors(xNew1(FactoryOpenDoorsCommand, unit));
		FactoryCloseDoorsCommand* closeDoors(xNew1(FactoryCloseDoorsCommand, unit));
		commands->AddCommand(GetTypeId<FactoryOpenDoorsCommand>(), openDoors, 0);
		commands->AddCommand(GetTypeId<FactoryCloseDoorsCommand>(), closeDoors, 0);
	}

	DllExclusive size_t FactoryKindDataSupplier::GetKindSpecificDataHandle() const
	{
		FactoryTypeData* data = xNew0(FactoryTypeData);
		return reinterpret_cast<size_t>(data);
	}

	DllExclusive void FactoryKindDataSupplier::DeleteHandle(size_t handle) const
	{
		FactoryTypeData* data = reinterpret_cast<FactoryTypeData*>(handle);
		xDelete(data);
	}

	DllExclusive size_t FactoryObjectDataSupplier::GetObjectSpecificDataHandle() const
	{
		FactoryObjectData* data = xNew0(FactoryObjectData);
		return reinterpret_cast<size_t>(data);
	}

	DllExclusive void FactoryObjectDataSupplier::DeleteHandle(size_t handle) const
	{
		FactoryObjectData* data = reinterpret_cast<FactoryObjectData*>(handle);
		xDelete(data);
	}
}