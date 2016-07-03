#pragma once

#include <IObjectStateSupplier.h>
#include <IUnitCommandSupplier.h>
#include <IKindSpecificDataSupplier.h>
#include <IObjectSpecificDataSupplier.h>
#include <UnitClassSupplier.h>

namespace FlagRTS
{
	class DllExclusive LogisticsCenterStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	class DllExclusive LogisticsCenterCommandSupplier : public IUnitCommandSupplier
	{
	public:
		void AddCommandsToUnit(Unit* unit);
	};

	class DllExclusive LogisticsCenterKindDataSupplier : public IKindSpecificDataSupplier
	{
	public:
		size_t GetKindSpecificDataHandle() const;
		void DeleteHandle(size_t handle) const;
	};

	class DllExclusive LogisticsCenterObjectDataSupplier : public IObjectSpecificDataSupplier
	{
	public:
		size_t GetObjectSpecificDataHandle() const;
		void DeleteHandle(size_t handle) const;
	};
}