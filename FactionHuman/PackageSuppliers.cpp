#include "PackageClassSuppliers.h"
#include "PackageStates.h"
#include "PackageData.h"
#include <PhysicalObject.h>

namespace FlagRTS
{
	DllExclusive void PackageStateSupplier::AddStatesToObject(SceneObject* object)
	{
		PhysicalObject* unit = (PhysicalObject*)object;
		object->AddState(GetTypeId<PackageMovingOnTransmissionBeltState>(), xNew1(PackageMovingOnTransmissionBeltState,unit));
		object->AddState(GetTypeId<PackageBeingTransportedByMineCraneState>(), xNew1(PackageBeingTransportedByMineCraneState,unit));
	}

	size_t PackageKindDataSupplier::GetKindSpecificDataHandle() const
	{
		PackageTypeData* data = xNew0(PackageTypeData);
		return reinterpret_cast<size_t>(data);
	}

	void PackageKindDataSupplier::DeleteHandle(size_t handle) const
	{
		PackageTypeData* data = reinterpret_cast<PackageTypeData*>(handle);
		xDelete(data);
	}

	size_t PackageObjectDataSupplier::GetObjectSpecificDataHandle() const
	{
		PackageObjectData* data = xNew0(PackageObjectData);
		return reinterpret_cast<size_t>(data);
	}

	void PackageObjectDataSupplier::DeleteHandle(size_t handle) const
	{
		PackageObjectData* data = reinterpret_cast<PackageObjectData*>(handle);
		xDelete(data);
	}
}