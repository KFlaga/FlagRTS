#pragma once

#include <boost\any.hpp>
#include "TypeId.h"

namespace FlagRTS
{
	typedef boost::any Any; // Alias for boost any for consistent naming

	template<typename T>
	struct ensure_refrence
	{
		typedef boost::mpl::if_<
            boost::is_reference<T>,
            T,
            boost::add_reference<T>::type
        >::type RefernceType;
	};

	/// Obtains data from 'Any'
	/**
		Uses boost::unsafe_any_cast. Preffered over boost::any_cast if value may
		be extracted in multiple dlls.
		\param any pointer to Any object
		\returns refrence to object stored in Any
	*/
	template<typename T>
	boost::add_reference<T> GetAnyData(Any* any)
	{
		return *boost::unsafe_any_cast<T>(any);
	}
	
	/// Obtains data from 'Any'
	/**
		Uses boost::unsafe_any_cast. Preffered over boost::any_cast if value may
		be extracted in multiple dlls.
		If reference is passed as T, then
		\param any reference to Any object
		\returns refrence to object stored in Any
	*/
	template<typename T>
	boost::add_reference<T> GetAnyData(Any& any)
	{
		return GetAnyData(&any);
	}
}