#pragma once

#include "DataTypes.h"

namespace FlagRTS
{
	// Add/removes unit classes depending on parsed string class names
	// Add/remove class should be set in constructor
	class UnitClassSupplier
	{
	protected:
		UnitClass _addclass;
		UnitClass _removeclass;

	public:
		UnitClassSupplier(const UnitClass add, const UnitClass remove) :
			_addclass(add),
			_removeclass(remove)
		{ }

		UnitClass ChangeUnitClass(const UnitClass base)
		{
			return (base | _addclass) & (~_removeclass);
		}
	};
}