#pragma once

namespace FlagRTS
{
// #include <type_traits> -> needs to be included if below macros are used

	/**
		Defines template struct for checking if class have named method.
		Usage of defined struct: HasMethod<Class>::value :
		evaluates to true if class 'Class' have method named 'Method'
		\param method name of method to check
	*/
#define DEFINE_MEMBER_METHOD_CHECK(method) \
	template <typename T>  \
	struct Has##method  \
	{ \
		struct dummy { }; \
		 \
		template <typename C, typename P> \
		static auto test(P * p) -> decltype(&C::method, std::true_type()); \
		 \
		template <typename, typename> \
		static std::false_type test(...); \
		 \
		typedef decltype(test<T, dummy>(nullptr)) type; \
		static const bool value = std::is_same<std::true_type, decltype(test<T, dummy>(nullptr))>::value; \
	};
	
	/**
		Defines template struct for checking if class have named method with given signature.
		Version for function with no arguments
		Usage of defined struct: HasMethod<Class>::value :
		evaluates to true if class 'Class' have method named 'Method' with given signature
		\param method name of method to check
		\param ReturnType type function should return
	*/
#define DEFINE_MEMBER_METHOD_SIGNATURE_CHECK_ARGS_0(method, ReturnType) \
	template <typename T>  \
	struct HasSignature##method##_##ReturnType  \
	{ \
		struct dummy { }; \
		 \
		template <typename C, typename P> \
		static auto test(T*) -> typename \
        std::is_same< \
            decltype( &C::method ), \
            ReturnType(C::*)()   \
			>::type;  \
		 \
		template <typename, typename> \
		static std::false_type test(...); \
		 \
		typedef decltype(test<T, dummy>(nullptr)) type; \
		static const bool value = std::is_same<std::true_type, decltype(test<T, dummy>(nullptr))>::value; \
	};

	/// Partial define not for use
#define _DEFINE_MEMBER_METHOD_INTERNAL_PART1(method, name)  \
	template <typename T>  \
	struct HasSignature##name \
	{ \
		struct dummy { }; \
		 \
		template <typename C, typename P> \
		static auto test(T*) -> typename \
        std::is_same< \
            decltype( &C::method ),
	
	/// Partial define not for use
#define _DEFINE_MEMBER_METHOD_INTERNAL_PART2  \
	>::type;  \
		 \
		template <typename, typename> \
		static std::false_type test(...); \
		 \
		typedef decltype(test<T, dummy>(nullptr)) type; \
		static const bool value = std::is_same<std::true_type, decltype(test<T, dummy>(nullptr))>::value; \
	};
	
	/**
		Defines template struct for checking if class have named method with given signature.
		Version for function with one argument
		Usage of defined struct: HasMethod<Class>::value :
		evaluates to true if class 'Class' have method named 'Method' with given signature
		\param method name of method to check
		\param ReturnType type function should return
		\param ArgN N-th argument type function should accept
	*/
#define DEFINE_MEMBER_METHOD_SIGNATURE_CHECK_ARGS_1(method, ReturnType, Arg1) \
	_DEFINE_MEMBER_METHOD_INTERNAL_PART1(method, method##_##ReturnType##_##Arg1) \
            ReturnType(C::*)(Arg1)   \
	_DEFINE_MEMBER_METHOD_INTERNAL_PART2 
	
	/**
		Defines template struct for checking if class have named method with given signature.
		Version for function with 2 arguments
		Usage of defined struct: HasMethod<Class>::value :
		evaluates to true if class 'Class' have method named 'Method' with given signature
		\param method name of method to check
		\param ReturnType type function should return
		\param ArgN N-th argument type function should accept
	*/
#define DEFINE_MEMBER_METHOD_SIGNATURE_CHECK_ARGS_2(method, ReturnType, Arg1, Arg2) \
	_DEFINE_MEMBER_METHOD_INTERNAL_PART1(method, method##_##ReturnType##_##Arg1##_##Arg2) \
            ReturnType(C::*)(Arg1, Arg2)   \
	_DEFINE_MEMBER_METHOD_INTERNAL_PART2 
	
	/**
		Defines template struct for checking if class have named method with given signature.
		Version for function with 3 arguments
		Usage of defined struct: HasMethod<Class>::value :
		evaluates to true if class 'Class' have method named 'Method' with given signature
		\param method name of method to check
		\param ReturnType type function should return
		\param ArgN N-th argument type function should accept
	*/
#define DEFINE_MEMBER_METHOD_SIGNATURE_CHECK_ARGS_3(method, ReturnType, Arg1, Arg2, Arg3) \
	_DEFINE_MEMBER_METHOD_INTERNAL_PART1(method, method##_##ReturnType##_##Arg1##_##Arg2##_##Arg3) \
            ReturnType(C::*)(Arg1, Arg2, Arg3)   \
	_DEFINE_MEMBER_METHOD_INTERNAL_PART2 

#define META_ENABLE_ENUM \
#include <boost\preprocessor\seq\enum.hpp> \
#include <boost\preprocessor\seq\for_each.hpp> \
#include "ArrayMap.h" \

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

/** Used internally in 'DEFINE_ENUM_PARSER' */
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
}