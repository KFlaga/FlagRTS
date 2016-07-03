#pragma once

#include <IObjectStateSupplier.h>
#include <IUnitCommandSupplier.h>
#include <IKindSpecificDataSupplier.h>
#include <IObjectSpecificDataSupplier.h>
#include <UnitClassSupplier.h>

namespace FlagRTS
{
	class DllExclusive PowerPlantStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	class DllExclusive PowerPlantKindDataSupplier : public IKindSpecificDataSupplier
	{
	public:
		size_t GetKindSpecificDataHandle() const;
		void DeleteHandle(size_t handle) const;
	};

	class DllExclusive PowerPlantObjectDataSupplier : public IObjectSpecificDataSupplier
	{
	public:
		size_t GetObjectSpecificDataHandle() const;
		void DeleteHandle(size_t handle) const;
	};

	class DllExclusive PowerPlantUnitClassSupplier : public UnitClassSupplier
	{
	public:
		PowerPlantUnitClassSupplier();
	};
}