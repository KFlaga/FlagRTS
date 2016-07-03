#pragma once

#include <new>
#include "MemoryChunk.h"
#include "SimpleType.h"

#ifdef _DEBUG
#include <typeinfo>
#endif

namespace FlagRTS
{
	// Allocator class is responsible for allocating proper amount of
	// memory ( via MemoryPool ) for object, costructing it and
	// deleting after its used
	// For object to be alloc
	class Allocator
	{
	public:
		Allocator();
		~Allocator();

		// Allocates memory neccessary for object and constructs it
		// Returns pointer to newly created object
		template<typename AllocType, typename... ConstructionParams>
		inline AllocType* Create(ConstructionParams... ctorParams)
		{
			// Alloc memory and call constructor
			AllocType* obj = Alloc<AllocType>();
			new (obj) AllocType(ctorParams...);
			return obj;
		}

		// Only calls constructor
		template<typename AllocType, typename... ConstructionParams>
		inline void CreateInPlace(AllocType* ptr, ConstructionParams... ctorParams)
		{
			new (ptr) AllocType(ctorParams...);
		}

		// Allocates memory neccessary for object and constructs it
		// Returns pointer to newly created object
		template<typename AllocType, typename... ConstructionParams>
		inline AllocType* CreateArray(unsigned int count, ConstructionParams... ctorParams)
		{
			// Alloc memory and call constructor for all objects
			AllocType* array = AllocArray<AllocType>(count);
			for(unsigned int i = 0; i < count; i++)
				new (&array[i]) AllocType(ctorParams...);
			return array;
		}
		
		// Allocates memory for object, but do not call constructor
		// for it ( so it should be used only for simple types )
		template<typename AllocType>
		inline AllocType* Alloc()
		{
			MemoryChunk* mem  = AllocInternal(sizeof(AllocType), 1);
			mem->Header.count = 1;
			return mem->operator AllocType*();
		}

		// Allocates memory for objects array, but do not call constructor
		// for it ( so it should be used only for simple types )
		template<typename AllocType>
		inline AllocType* AllocArray(unsigned int count)
		{
			MemoryChunk* mem = AllocInternal(sizeof(AllocType), count);
			mem->Header.count = count;
			return mem->operator AllocType*();
		}
		
		// Destroys object created by Create() function and frees memory
		template<typename AllocType>
		inline void Destroy(AllocType* ptr)
		{
			// Call destructor and dealloc memory
			ptr->~AllocType();
			Dealloc<AllocType>(ptr);
		}

		// Destroys object created by Create() function and frees memory
		template<typename AllocType>
		inline void DestroyArray(AllocType* ptr)
		{
			// Call destructor for all objects and dealloc memory
			const unsigned int arraySize = MemoryChunk::FromPtr<AllocType>(ptr)->Header.count;
			for(unsigned int i = 0; i < arraySize; i++)
				ptr[i].~AllocType();
			Dealloc<AllocType>(ptr);
		}

		// Frees memory allocated by Alloc() function
		template<typename AllocType>
		inline void Dealloc(AllocType* ptr)
		{
			DeallocInternal(MemoryChunk::FromPtr(ptr));
		}

		// Allocates memory neccessary for object and constructs it
		// Returns pointer to newly created object
		template<typename AllocType, typename... ConstructionParams>
		inline AllocType* Create_d(const char* file, int line, ConstructionParams... ctorParams)
		{
			// Alloc memory and call constructor
			AllocType* obj = Alloc_d<AllocType>(file, line);
			new (obj) AllocType(ctorParams...);
			return obj;
		}

		// Only calls constructor
		template<typename AllocType, typename... ConstructionParams>
		inline void CreateInPlace_d(AllocType* ptr, const char* file, int line, ConstructionParams... ctorParams)
		{
			new (ptr) AllocType(ctorParams...);
		}

		// Allocates memory neccessary for object and constructs it
		// Returns pointer to newly created object
		template<typename AllocType, typename... ConstructionParams>
		inline AllocType* CreateArray_d(const unsigned int count, const char* file, int line, ConstructionParams... ctorParams)
		{
			// Alloc memory and call constructor for all objects
			AllocType* array = AllocArray_d<AllocType>(count, file, line);
			for(unsigned int i = 0; i < count; i++)
				new (&array[i]) AllocType(ctorParams...);
			return array;
		}
		
		// Allocates memory for object, but do not call constructor
		// for it ( so it should be used only for simple types )
		template<typename AllocType>
		inline AllocType* Alloc_d(const char* file, int line)
		{
			MemoryChunk* mem  = AllocInternal_d(sizeof(AllocType), 1, file, line, typeid(AllocType).name());
			mem->Header.count = 1;
			return mem->operator AllocType*();
		}

		// Allocates memory for objects array, but do not call constructor
		// for it ( so it should be used only for simple types )
		template<typename AllocType>
		inline AllocType* AllocArray_d(const unsigned int count, const char* file, int line)
		{
			MemoryChunk* mem = AllocInternal_d(sizeof(AllocType), count, file, line, typeid(AllocType).name());
			mem->Header.count = count;
			return mem->operator AllocType*();
		}
		
		// Destroys object created by Create() function and frees memory
		template<typename AllocType>
		inline void Destroy_d(AllocType* ptr, const char* file, int line)
		{
			// Call destructor and dealloc memory
			ptr->~AllocType();
			Dealloc_d<AllocType>(ptr, file, line);
		}

		// Destroys object created by Create() function and frees memory
		template<typename AllocType>
		inline void DestroyArray_d(AllocType* ptr, const char* file, int line)
		{
			// Call destructor for all objects and dealloc memory
			const unsigned int arraySize = MemoryChunk::FromPtr<AllocType>(ptr)->Header.count;
			for(unsigned int i = 0; i < arraySize; ++i)
				ptr[i].~AllocType();
			Dealloc_d<AllocType>(ptr, file, line);
		}

		// Frees memory allocated by Alloc() function
		template<typename AllocType>
		inline void Dealloc_d(AllocType* ptr, const char* file, int line)
		{
			DeallocInternal_d(MemoryChunk::FromPtr(ptr), file, line, typeid(AllocType).name());
		}

		// Copies allocator
		virtual Allocator* GetAllocator() = 0;

		// Frees copied allocator 
		virtual void FreeAllocator(Allocator* allocator) = 0;

		// Ensures that memory for at least 'count' number of object
		// is enabled
		template<typename AllocType>
		void ReserveMemory(const int count)
		{
			ReserveInternal(sizeof(AllocType), count);
		}
		
		//void ReserveMemory(int size);

	protected:
		virtual MemoryChunk* AllocInternal(const size_t size, const unsigned int count) = 0;
		virtual	void DeallocInternal(MemoryChunk*) = 0;

		virtual void ReserveInternal(const size_t size, const unsigned int count) = 0;

		virtual MemoryChunk* AllocInternal_d(const size_t size, const unsigned int count, const char* file, int line, const char* type);
		virtual void DeallocInternal_d(MemoryChunk*, const char* file, int line, const char* type);
	};
}