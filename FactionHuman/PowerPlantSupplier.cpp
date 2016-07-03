#include "PowerPlantSuppliers.h"
#include "PowerPlantStates.h"
#include "PowerPlantData.h"
#include <Unit.h>
#include <UnitClass.h>

namespace FlagRTS
{
	DllExclusive void PowerPlantStateSupplier::AddStatesToObject(SceneObject* object)
	{
		Unit* unit = (Unit*)object;
		object->AddState(SceneObjectStates::Idle, xNew1(PowerPlantWorkingState,unit));
		object->AddState(GetTypeId<PowerPlantWorkingState>(), xNew1(PowerPlantWorkingState,unit));
	}

	DllExclusive size_t PowerPlantKindDataSupplier::GetKindSpecificDataHandle() const
	{
		PowerPlantTypeData* data = xNew0(PowerPlantTypeData);
		return reinterpret_cast<size_t>(data);
	}

	DllExclusive void PowerPlantKindDataSupplier::DeleteHandle(size_t handle) const
	{
		PowerPlantTypeData* data = reinterpret_cast<PowerPlantTypeData*>(handle);
		xDelete(data);
	}

	DllExclusive size_t PowerPlantObjectDataSupplier::GetObjectSpecificDataHandle() const
	{
		PowerPlantObjectData* data = xNew0(PowerPlantObjectData);
		return reinterpret_cast<size_t>(data);
	}

	DllExclusive void PowerPlantObjectDataSupplier::DeleteHandle(size_t handle) const
	{
		PowerPlantObjectData* data = reinterpret_cast<PowerPlantObjectData*>(handle);
		xDelete(data);
	}

	using namespace UnitClasses;

	PowerPlantUnitClassSupplier::PowerPlantUnitClassSupplier() :
		UnitClassSupplier(
		AddClass(ComplexClasses::Building, UnitClasses::ResourceGenerator),
			ComplexClasses::None)
	{ }
}