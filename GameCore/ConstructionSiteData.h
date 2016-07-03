#pragma once

#include "IObjectSpecificDataSupplier.h"
#include "IKindSpecificDataSupplier.h"

namespace FlagRTS
{
	class Unit;

	struct ConstructionSiteTypeData
	{
		float TickLength; // Construction update interval

		ConstructionSiteTypeData() :
			TickLength(500.f)
		{ }
	};

	class ConstructionSiteTypeDataSupplier : public IKindSpecificDataSupplier
	{
	public:
		size_t GetKindSpecificDataHandle() const;
		void DeleteHandle(size_t handle) const;
	};

	struct ConstructionSiteObjectData
	{
		Unit* Building;

		ConstructionSiteObjectData() :
			Building(0)
		{ }
	};

	class ConstructionSiteObjectDataSupplier : public IObjectSpecificDataSupplier
	{
	public:
		size_t GetObjectSpecificDataHandle() const;
		void DeleteHandle(size_t handle) const;
	};
}