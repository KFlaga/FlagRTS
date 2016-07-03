#include "TypeId.h"
#include "HashMap.h"
#include <typeindex>

namespace FlagRTS
{
	// Initialize in main() 
	TypeHelper* TypeHelper::GlobalTypeHelper = 0;

	struct TypeHasher
	{
		uint32 operator()(const std::type_index& typeInfo)
		{
			return (uint32)typeInfo.hash_code();
		}
	};

	struct TypeHelperInternal
	{
		HashedMap<std::type_index, TypeId, TypeHasher> TypeIdMap;
		size_t Counter;
	};

	TypeHelper::TypeHelper() :
		_internal(new TypeHelperInternal())
	{
		_internal->Counter = 1000;
	}

	TypeHelper::~TypeHelper()
	{
		delete _internal;
	}

	TypeId TypeHelper::GetNextId(const std::type_info& typeInfo)
	{
		auto typeIt = _internal->TypeIdMap.find( std::type_index(typeInfo) );
		if( typeIt != _internal->TypeIdMap.end() )
		{
			return typeIt->Value;
		}
		++_internal->Counter;
		_internal->TypeIdMap[std::type_index(typeInfo)] = _internal->Counter;
		return _internal->Counter;

	}
}