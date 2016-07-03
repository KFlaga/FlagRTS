#pragma once

#include <TypeId.h>
#include <string>

namespace FlagRTS
{
	namespace ObjectTypeHelpers
	{
		const char* FindTypeFromId(TypeId type);
		TypeId FindTypeFromString(const std::string& type);
	};
}