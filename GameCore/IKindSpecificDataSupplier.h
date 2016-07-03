#pragma once

#include <TypeDefs.h>

namespace FlagRTS
{
	// Returns handle to kind specific data
	class IKindSpecificDataSupplier
	{
	public:
		virtual size_t GetKindSpecificDataHandle() const = 0;
		virtual void DeleteHandle(size_t handle) const = 0;
	};
}