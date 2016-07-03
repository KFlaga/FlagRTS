#pragma once

#include "AllocatorStdWrapper.h"
#include <map>

namespace FlagRTS
{
	template <typename KeyType, typename ValType, typename LessComparer = std::less<KeyType>>
	struct KeyValMap : public std::map<
		KeyType, 
		ValType, 
		LessComparer, 
		AllocatorStdWrapper<std::pair<KeyType,ValType>>>
	{ };
}