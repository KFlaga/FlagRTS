#pragma once

namespace FlagRTS
{
	// SimpleTypes are constructor-less types - they are allocated
	// differently ( no constructor is called on them )
	// If type is regisered as simple then IsSimple == 1, so
	// it's easy to check
	// To register type as simple SIMPLETYPE macro with type must be called

	template<class T> struct SimpleType 
	{
		enum {IsSimple = 0};
	};
	template<class T> struct SimpleType<T *> 
	{
		enum {IsSimple = 1};
	};
	template<class T> struct SimpleType<const T> 
	{
		enum {IsSimple = SimpleType<T>::IsSimple};
	};
	template<class T, size_t n> struct SimpleType<T[n]> 
	{
		enum {IsSimple = SimpleType<T>::IsSimple};
	};

#define SIMPLETYPE(T)				\
	template<> struct SimpleType<T> \
	{								\
		enum {IsSimple = 1};		\
	}
	// Only primitives can be registered simple if uncommented
	//union							\
	//{								\
	//	T t;						\
	//} catcherr;					\
	//enum {IsSimple = 1};			\
	//}

	SIMPLETYPE (bool);
	SIMPLETYPE (char);
	SIMPLETYPE (unsigned char);
	SIMPLETYPE (short);
	SIMPLETYPE (unsigned short);
	SIMPLETYPE (int);
	SIMPLETYPE (unsigned int);
	SIMPLETYPE (long);
	SIMPLETYPE (unsigned long);
	SIMPLETYPE (float);
	SIMPLETYPE (double);
	SIMPLETYPE (long double);

	SIMPLETYPE (long long);
	SIMPLETYPE (unsigned long long);

#define IsTypeSimple(T) ((bool) SimpleType<T>::IsSimple)

	template<class T, bool S = IsTypeSimple (T)> struct EnsureSimple;
	template<class T> struct EnsureSimple<T, true> {};
}