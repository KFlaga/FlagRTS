#include "LogisticsCenterClassSuppliers.h"
#include "LogisticsCenterStates.h"
#include "LogisticsCenterOrders.h"
#include "LogisticsCenterData.h"
#include <Unit.h>
#include <UnitClass.h>

namespace FlagRTS
{
	DllExclusive void LogisticsCenterStateSupplier::AddStatesToObject(SceneObject* object)
	{
		Unit* unit = (Unit*)object;
	}

	DllExclusive void LogisticsCenterCommandSupplier::AddCommandsToUnit(Unit* unit)
	{
		auto commands = unit->GetCommands();
	}

	DllExclusive size_t LogisticsCenterKindDataSupplier::GetKindSpecificDataHandle() const
	{
		LogisticsCenterTypeData* data = xNew0(LogisticsCenterTypeData);
		return reinterpret_cast<size_t>(data);
	}

	DllExclusive void LogisticsCenterKindDataSupplier::DeleteHandle(size_t handle) const
	{
		LogisticsCenterTypeData* data = reinterpret_cast<LogisticsCenterTypeData*>(handle);
		xDelete(data);
	}

	DllExclusive size_t LogisticsCenterObjectDataSupplier::GetObjectSpecificDataHandle() const
	{
		LogisticsCenterObjectData* data = xNew0(LogisticsCenterObjectData);
		return reinterpret_cast<size_t>(data);
	}

	DllExclusive void LogisticsCenterObjectDataSupplier::DeleteHandle(size_t handle) const
	{
		LogisticsCenterObjectData* data = reinterpret_cast<LogisticsCenterObjectData*>(handle);
		xDelete(data);
	}
}