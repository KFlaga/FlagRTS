#pragma once

#include "AllocatorStdWrapper.h"
#include <vector>

namespace FlagRTS
{
	template <typename T>
	class Array : public std::vector<T/*, AllocatorStdWrapper<T>*/>
	{ 
	public:
		typedef std::vector<T/*, AllocatorStdWrapper<T>*/> BaseTypeTmp;
		typedef vector BaseType;

		Array() :
			BaseType()
		{ }

		Array(const BaseTypeTmp& other) :
			BaseType(other)
		{ }

		Array(const Array& other) :
			BaseType(other)
		{ }

		Array(BaseTypeTmp&& other) :
			BaseType(other)
		{ }

		Array(Array&& other) :
			BaseType(other)
		{ }

		Array(size_t n) :
			BaseType(n)
		{ }

		Array(size_t n, const T& val) :
			BaseType(n, val)
		{ }

		void removeAt(size_t pos)
		{
			_ASSERT(pos < size() - 1);
			for(; pos < size()-1; ++pos)
			{
				this->operator[](pos) = this->operator[](pos+1);
			}
			this->pop_back();
		}

		void remove(const T& val)
		{
			size_t pos = 0;
			for(; pos < size(); ++pos)
			{
				if( val == this->operator[](pos) )
					break;
			}
			if(pos < size())
			{
				for(; pos < size()-1; ++pos)
				{
					this->operator[](pos) = this->operator[](pos+1);
				}
				this->pop_back();
			}
		}

		// Return 1st index with given value, or -1 if not found
		int getFirstIndexOf(const T& val) const
		{
			size_t pos = 0;
			for(; pos < size(); ++pos)
			{
				if( val == this->operator[](pos) )
					return pos;
			}
			return -1;
		}
		
		// Return last index with given value, or -1 if not found
		int getLastIndexOf(const T& val) const
		{
			size_t pos = 0;
			size_t lastPos = -1;
			for(; pos < size(); ++pos)
			{
				if( val == this->operator[](pos) )
				{
					lastPos = pos;
				}
			}
			return lastPos;
		}
	};

	template<>
	class Array<bool> : public std::vector<bool>
	{ 
	public:
		typedef std::vector<bool> BaseTypeTmp;
		typedef vector BaseType;

		Array() :
			BaseType()
		{ }

		Array(const BaseTypeTmp& other) :
			BaseType(other)
		{ }

		Array(const Array& other) :
			BaseType(other)
		{ }

		Array(BaseTypeTmp&& other) :
			BaseType(other)
		{ }

		Array(Array&& other) :
			BaseType(other)
		{ }

		Array(size_t n) :
			BaseType(n)
		{ }

		Array(size_t n, bool val) :
			BaseType(n, val)
		{ }
	};

#define DLLEXPORT_ARRAY( type ) \
	template class DllApi std::allocator< type >; \
	template class DllApi Array<type>;

}