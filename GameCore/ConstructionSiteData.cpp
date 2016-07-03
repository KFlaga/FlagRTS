#include "ConstructionSiteData.h"
#include <Memory.h>

namespace FlagRTS
{
	size_t ConstructionSiteTypeDataSupplier::GetKindSpecificDataHandle() const
	{
		ConstructionSiteTypeData* data = xNew0(ConstructionSiteTypeData);
		return reinterpret_cast<size_t>(data);
	}

	void ConstructionSiteTypeDataSupplier::DeleteHandle(size_t handle) const
	{
		ConstructionSiteTypeData* data = reinterpret_cast<ConstructionSiteTypeData*>(handle);
		xDelete(data);
	}

	size_t ConstructionSiteObjectDataSupplier::GetObjectSpecificDataHandle() const
	{
		ConstructionSiteObjectData* data = xNew0(ConstructionSiteObjectData);
		return reinterpret_cast<size_t>(data);
	}

	void ConstructionSiteObjectDataSupplier::DeleteHandle(size_t handle) const
	{
		ConstructionSiteObjectData* data = reinterpret_cast<ConstructionSiteObjectData*>(handle);
		xDelete(data);
	}
}