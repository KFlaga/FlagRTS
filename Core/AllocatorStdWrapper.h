#pragma once

#include <xmemory0>
#include "Memory.h"

namespace FlagRTS
{
	template<class type>
	class AllocatorStdWrapper : public std::allocator<type>
	{
	public:
		AllocatorStdWrapper() _THROW0() :
			allocator()
		{ }

		AllocatorStdWrapper(const AllocatorStdWrapper<type>& other) _THROW0() :
			allocator()
		{
		}

		template<class OtherType>
		AllocatorStdWrapper(const AllocatorStdWrapper<OtherType>& other) _THROW0() :
			allocator()
		{
		}

		template<class OtherType>
		allocator<type>& operator=(const AllocatorStdWrapper<OtherType>& other)
		{	
			return (*this);
		}

		void deallocate(pointer _Ptr, size_type)
		{	// deallocate object at _Ptr, ignore size
			ndsDelete( _Ptr );
		}

		pointer allocate(size_type _Count)
		{	// allocate array of _Count elements
			return ndNewArray0(type, _Count);
		}

		pointer allocate(size_type _Count, const void *)
		{	// allocate array of _Count elements, ignore hint
			return ndNewArray0(type, _Count);
		}

		void construct(type *_Ptr)
		{	// default construct object at _Ptr
			::new ((void *)_Ptr) type();
		}

		void construct(type *_Ptr, const type& _Val)
		{	// construct object at _Ptr with value _Val
			::new ((void *)_Ptr) type(_Val);
		}

#define _ALLOC_MEMBER_CONSTRUCT( \
	TEMPLATE_LIST, PADDING_LIST, LIST, COMMA, CALL_OPT, X2, X3, X4) \
	template<class _Objty COMMA LIST(_CLASS_TYPE)> \
	void construct(_Objty *_Ptr COMMA LIST(_TYPE_REFREF_ARG)) \
		{	/* construct _Objty(_Types...) at _Ptr */ \
		::new ((void *)_Ptr) _Objty(LIST(_FORWARD_ARG)); \
		}

		_VARIADIC_EXPAND_0X(_ALLOC_MEMBER_CONSTRUCT, , , , )
#undef _ALLOC_MEMBER_CONSTRUCT

		template<class _Uty>
		void destroy(_Uty *_Ptr)
		{	// destroy object at _Ptr
			_Ptr->~_Uty();
		}

		size_t max_size() const _THROW0()
		{	// estimate maximum array size
			return ((size_t)(-1) / sizeof (type));
		}

		template <class U>
		struct rebind
		{
			typedef AllocatorStdWrapper<U> other;
		};
	};
}