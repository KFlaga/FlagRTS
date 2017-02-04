#pragma once

#include <boost\preprocessor\seq\enum.hpp>
#include <boost\preprocessor\seq\for_each.hpp>
#include <boost\preprocessor\stringize.hpp>
#include "ArrayMap.h"

/**
	Macro used to define enum with Parse method.
	Enum and method is enclosed in own namespace.
	Let 'Name' be enum passed name, then namespace name is 'Names' and
	enum fully qualified type is 'FlagRTS::Names::NameType'.
	Typedef outside of enum space allow to acces enum as 'FlagRTS::Name'

	\param enum_name name of enum
	\param enum_type interger type of enum
	\param enum_seq enum seqence, each value enclosed in parenthesis, may contain expressions

	Example usage:
	DEFINE_ENUM_WITH_PARSE(MyEnum, int32, (Val1=0)(Val2=Val1+3))
*/
#define DEFINE_ENUM_WITH_PARSE(enum_name, enum_type, enum_seq) \
	namespace FlagRTS { \
		namespace enum_name##s \
		{ \
			enum enum_name##Type : enum_type  \
			{ \
				BOOST_PP_SEQ_ENUM(enum_seq) \
			}; \
			enum_name##Type Parse##enum_name(const char* name); \
		} \
		typedef enum_name##s::enum_name##Type enum_name \
	}

/// Used internally in 'DEFINE_ENUM_PARSER'
#define INSERT_TO_ENUM_MAP(i, data, elem) map.insert(#elem, elem);

/**
	Macro used to define enum's Parse method for enums defined by 'DEFINE_ENUM_WITH_PARSE'
	If enum is not defined with that macro, then it must have parse methos same as
	one created with that macro

	\param enum_name name of enum
	\param enum_seq enum seqence, each value enclosed in parenthesis, may NOT contain expressions
*/
#define DEFINE_ENUM_PARSER(enum_name, enum_seq) \
	namespace FlagRTS { \
	namespace enum_name##s \
	{ \
		FlagRTS::ArrayMaps<enum_name##Type>::KeyCCString& GetEnumNamesMap() \
		{ \
			static FlagRTS::ArrayMaps<enum_name##Type>::KeyCCString map; \
			BOOST_PP_SEQ_FOR_EACH(  \
                INSERT_TO_ENUM_MAP,  \
                "", \
                enum_seq ) \
			return map; \
		} \
		enum_name##Type Parse##enum_name(const char* name) \
		{ \
			static FlagRTS::ArrayMaps<enum_name##Type>::KeyCCString& map = GetEnumNamesMap(); \
			auto it = map.find(name); \
			if(map.isNotEnd(it)) \
				return it->Value; \
			return (enum_name##Type)0;\
		} \
	} \
	}
		