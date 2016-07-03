#pragma once

#include <TypeDefs.h>

namespace FlagRTS
{
	// Returns handle to object specific data
	class IObjectSpecificDataSupplier
	{
	public:
		virtual size_t GetObjectSpecificDataHandle() const = 0;
		virtual void DeleteHandle(size_t handle) const = 0;
	};
}