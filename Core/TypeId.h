#pragma once

#include <typeinfo>

namespace FlagRTS
{
	// TypeId iss identifier unique to type, allowing
	// for run-time identification/comparsion of given type
	// Its unique for every type, as in fact its address of static function
	// of template type_helper struct
	typedef size_t TypeId;

	//template<typename T>
	//struct type_helper { static void id() { } };
	//// Returns TypeId of type in template parameter
	//template<typename T>
	//TypeId GetTypeId() { return reinterpret_cast<size_t>(&type_helper<T>::id); }
	//// Returns TypeId of object passed as parameter
	//template<typename T>
	//TypeId GetTypeId(const T&) { return reinterpret_cast<size_t>(&type_helper<T>::id); }

	// ABOVE CANNOT BE USED ACROSS DLLs

	struct TypeHelperInternal;
	struct TypeHelper
	{
	public:
		static TypeHelper* GlobalTypeHelper; // Same pointer must be used in all dlls

	private:
		TypeHelperInternal* _internal;

	public:
		TypeHelper();
		~TypeHelper();
		TypeId GetNextId(const std::type_info& typeInfo); // Ensures that multiple calls with same type returns same id
	};

	/// Returns TypeId of type in template parameter
	template<typename T>
	TypeId GetTypeId() 
	{
		static TypeId id = TypeHelper::GlobalTypeHelper->GetNextId(typeid(T)); // id is computed only 1 time for type ( for each dll )
		return id; 
	}
	/// Returns TypeId of object passed as parameter
	template<typename T>
	TypeId GetTypeId(const T&) 
	{ 
		return GetTypeId<T>();
	}



	// TypeId of this struct is considered invalid
	struct bad_type { };
#define GetInvalidType() GetTypeId<bad_type>()
#define IsValidType(type) type != GetInvalidType()
}