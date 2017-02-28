#pragma once

#include "Allocator.h"

namespace FlagRTS
{
	// Main allocator used through out the program
	extern Allocator* Malloc;
}

namespace FlagRTS
{

	// No debug info version

#define ndNew(T) \
	IsTypeSimple(T) ?									\
	Malloc->Alloc<T>() :			\
	Malloc->Create<T>()	

#define ndNew0(T) Malloc->Create<T>()
#define ndNew1(T,a1) Malloc->Create<T>(a1)
#define ndNew2(T,a1,a2) Malloc->Create<T>(a1, a2)
#define ndNew3(T,a1,a2,a3) Malloc->Create<T>(a1, a2, a3)
#define ndNew4(T,a1,a2,a3,a4) Malloc->Create<T>(a1, a2, a3, a4)
#define ndNew5(T,a1,a2,a3,a4, a5) Malloc->Create<T>(a1, a2, a3, a4, a5)
#define ndNew6(T,a1,a2,a3,a4, a5, a6) Malloc->Create<T>(a1, a2, a3, a4, a5, a6)

#define ndAlloc(T) Malloc->Alloc<T>()

#define ndNewArray(T, size) \
	IsTypeSimple(T) ?										\
	Malloc->AllocArray<T>(size) :	\
	Malloc->CreateArray<T>(size)		

#define ndNewArray0(T, size) Malloc->CreateArray<T>(size)
#define ndNewArray1(T, size, a1) Malloc->CreateArray<T>(size, a1)
#define ndNewArray2(T, size, a1, a2) Malloc->CreateArray<T>(size, a1, a2)
#define ndAllocArray(T, size) Malloc->AllocArray<T>(size)

#define ndsDelete(ptr) Malloc->Dealloc(ptr)
#define ndsDeleteSafe(ptr) if(ptr) { Malloc->Dealloc(ptr); ptr = 0; }

#define ndDelete(ptr) Malloc->Destroy(ptr)
#define ndDeleteSafe(ptr) if(ptr) { Malloc->Destroy(ptr); ptr = 0; }

#define ndsDeleteArray(ptr) Malloc->Dealloc(ptr)

#define ndDeleteArray(ptr) Malloc->DestroyArray(ptr)

#define nduDelete(T,ptr)		\
	IsTypeSimple(T) ?		\
	ndsDelete(ptr) :	\
	ndDelete(ptr)

#define nduDeleteSafe(T,ptr)		\
	IsTypeSimple(T) ?		\
	ndsDeleteSafe(ptr) :	\
	ndDeleteSafe(ptr)


#define nduDeleteArray(T,ptr)		\
	IsTypeSimple(T) ?		\
	ndsDeleteArray(ptr) :	\
	ndDeleteArray(ptr)	


#ifndef _DEBUG

#define New(T) \
	IsTypeSimple(T) ?									\
	Malloc->Alloc<T>(]) :			\
	Malloc->Create<T>()	

#define xNew0(T) Malloc->Create<T>()
#define xNew(T, args) Malloc->Create<T>(args)
#define xNew1(T,a1) Malloc->Create<T>(a1)
#define xNew2(T,a1,a2) Malloc->Create<T>(a1, a2)
#define xNew3(T,a1,a2,a3) Malloc->Create<T>(a1, a2, a3)
#define xNew4(T,a1,a2,a3,a4) Malloc->Create<T>(a1, a2, a3, a4)
#define xNew5(T,a1,a2,a3,a4, a5) Malloc->Create<T>(a1, a2, a3, a4, a5)
#define xNew6(T,a1,a2,a3,a4, a5, a6) Malloc->Create<T>(a1, a2, a3, a4, a5, a6)

#define xAlloc(T) Malloc->Alloc<T>()

#define NewArray(T, size) \
	IsTypeSimple(T) ?										\
	Malloc->AllocArray<T>(size) :	\
	Malloc->CreateArray<T>(size)		

#define xNewArray0(T, size) Malloc->CreateArray<T>(size)
#define xNewArray1(T, size, arg1) Malloc->CreateArray<T>(size, arg1)
#define xNewArray2(T, size, arg1, arg2) Malloc->CreateArray<T>(size, arg1, arg2)
#define sNewArray(T, size) Malloc->AllocArray<T>(size)

#define sDelete(ptr) Malloc->Dealloc(ptr)
#define sDeleteSafe(ptr) if(ptr) { Malloc->Dealloc(ptr); ptr = 0; }

#define xDelete(ptr) Malloc->Destroy(ptr)
#define xDeleteSafe(ptr) if(ptr) { Malloc->Destroy(ptr); ptr = 0; }

#define sDeleteArray(ptr) Malloc->Dealloc(ptr)

#define xDeleteArray(ptr) Malloc->DestroyArray(ptr)

#define uDelete(T,ptr)		\
	IsTypeSimple(T) ?		\
	sDelete(ptr) :	\
	xDelete(ptr)

#define uDeleteSafe(T,ptr)		\
	IsTypeSimple(T) ?		\
	sDeleteSafe(ptr) :	\
	xDeleteSafe(ptr)


#define uDeleteArray(T,ptr)		\
	IsTypeSimple(T) ?		\
	sDeleteArray(ptr) :	\
	xDeleteArray(ptr)	


#else

	// Final global function for allocation of objects
	//

#define New(T) \
	IsTypeSimple(T) ?									\
	Malloc->Alloc_d<T>(__FILE__, __LINE__) :			\
	Malloc->Create_d<T>(__FILE__, __LINE__)	

#define xNew0(T) Malloc->Create_d<T>(__FILE__, __LINE__)
#define xNew1(T,a1) Malloc->Create_d<T>(__FILE__, __LINE__, a1)
#define xNew2(T,a1,a2) Malloc->Create_d<T>(__FILE__, __LINE__, a1, a2)
#define xNew3(T,a1,a2,a3) Malloc->Create_d<T>(__FILE__, __LINE__, a1, a2, a3)
#define xNew4(T,a1,a2,a3,a4) Malloc->Create_d<T>(__FILE__, __LINE__, a1, a2, a3, a4)
#define xNew5(T,a1,a2,a3,a4, a5) Malloc->Create_d<T>(__FILE__, __LINE__, a1, a2, a3, a4, a5)
#define xNew6(T,a1,a2,a3,a4, a5, a6) Malloc->Create_d<T>(__FILE__, __LINE__, a1, a2, a3, a4, a5, a6)

#define xAlloc(T) Malloc->Alloc_d<T>(__FILE__,__LINE__)

#define NewArray(T, size) \
	IsTypeSimple(T) ?										\
	Malloc->AllocArray_d<T>(size, __FILE__, __LINE__) :	\
	Malloc->CreateArray_d<T>(size, __FILE__, __LINE__)		

#define xNewArray0(T, size) Malloc->CreateArray_d<T>(size, __FILE__, __LINE__)
#define xNewArray1(T, size, args) Malloc->CreateArray_d<T>(size, __FILE__, __LINE__, args)
#define xNewArray2(T, size, a1, a2) Malloc->CreateArray_d<T>(size, __FILE__, __LINE__, a1, a2)
#define sNewArray(T, size) Malloc->AllocArray_d<T>(size, __FILE__, __LINE__)

#define sDelete(ptr) Malloc->Dealloc_d(ptr, __FILE__, __LINE__)
#define sDeleteSafe(ptr) if(ptr) { Malloc->Dealloc_d(ptr, __FILE__, __LINE__); ptr = 0; }

#define xDelete(ptr) Malloc->Destroy_d(ptr, __FILE__, __LINE__)
#define xDeleteSafe(ptr) if(ptr) { Malloc->Destroy_d(ptr, __FILE__, __LINE__); ptr = 0; }

#define sDeleteArray(ptr) Malloc->Dealloc_d(ptr, __FILE__, __LINE__)

#define xDeleteArray(ptr) Malloc->DestroyArray_d(ptr, __FILE__, __LINE__)

#define uDelete(T,ptr)		\
	IsTypeSimple(T) ?		\
	sDelete(ptr) :	\
	xDelete(ptr)

#define uDeleteSafe(T,ptr)		\
	IsTypeSimple(T) ?		\
	sDeleteSafe(ptr) :	\
	xDeleteSafe(ptr)

#define uDeleteArray(T,ptr)		\
	IsTypeSimple(T) ?		\
	sDeleteArray(ptr) :	\
	xDeleteArray(ptr)	


#endif
}