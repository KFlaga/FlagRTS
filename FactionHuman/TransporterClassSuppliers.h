#pragma once

#include <IObjectStateSupplier.h>
#include <IUnitCommandSupplier.h>
#include <IKindSpecificDataSupplier.h>
#include <IObjectSpecificDataSupplier.h>
#include <UnitClassSupplier.h>

namespace FlagRTS
{
	class DllExclusive TransporterStateSupplier : public IObjectStateSupplier
	{
	public:
		void AddStatesToObject(SceneObject* object);
	};

	class DllExclusive TransporterCommandSupplier : public IUnitCommandSupplier
	{
	public:
		void AddCommandsToUnit(Unit* unit);
	};

	class DllExclusive TransporterKindDataSupplier : public IKindSpecificDataSupplier
	{
	public:
		size_t GetKindSpecificDataHandle() const;
		void DeleteHandle(size_t handle) const;
	};

	class DllExclusive TransporterObjectDataSupplier : public IObjectSpecificDataSupplier
	{
	public:
		size_t GetObjectSpecificDataHandle() const;
		void DeleteHandle(size_t handle) const;
	};

	class DllExclusive TransporterUnitClassSupplier : public UnitClassSupplier
	{
	public:
		TransporterUnitClassSupplier();
	};
}