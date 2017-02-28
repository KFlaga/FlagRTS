#pragma once

#include "DataTypes.h"
#include <HashMap.h>
#include <Any.h>

namespace FlagRTS
{
	/// Wrapper around hash map (to avoid boost inclusion in half the files)  
	struct NamedPropertiesMap : public HashedMaps<Any>::KeyCCString
	{
	};
}