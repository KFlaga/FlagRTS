#pragma once

#include <ObjectHandle.h>
#include <DataTypes.h>

namespace FlagRTS
{
	struct DllExclusive PackageTypeData
	{
		PackageTypeData();
	};

	struct DllExclusive PackageObjectData
	{
		int StoredMetal;
		PackageObjectData();
	};
}