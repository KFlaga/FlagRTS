#pragma once

#include <IObjectStateSupplier.h>
#include <IUnitCommandSupplier.h>
#include <IKindSpecificDataSupplier.h>
#include <IObjectSpecificDataSupplier.h>
#include <UnitClassSupplier.h>

namespace FlagRTS
{
	class DllExclusive FactoryStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	class DllExclusive FactoryCommandSupplier : public IUnitCommandSupplier
	{
	public:
		void AddCommandsToUnit(Unit* unit);
	};

	class DllExclusive FactoryKindDataSupplier : public IKindSpecificDataSupplier
	{
	public:
		size_t GetKindSpecificDataHandle() const;
		void DeleteHandle(size_t handle) const;
	};

	class DllExclusive FactoryObjectDataSupplier : public IObjectSpecificDataSupplier
	{
	public:
		size_t GetObjectSpecificDataHandle() const;
		void DeleteHandle(size_t handle) const;
	};
}