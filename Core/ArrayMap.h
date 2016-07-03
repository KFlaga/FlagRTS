#pragma once

#include "TypeDefs.h"
#include "Array.h"
#include "Iterators.h"

namespace FlagRTS
{
	// Simple, unsorted array of key-val pairs
	template<typename KeyT, typename ValT, typename IsKeyEqual = std::equal_to<KeyT>>
	class ArrayMap
	{
	public:
		typedef KeyT KeyType;
		typedef ValT ValueType;
		typedef typename ConditinalType< sizeof(KeyType) <= sizeof(size_t), 
			KeyType, KeyType&>::type KeyTypeParam;
		typedef typename ConditinalType< sizeof(ValueType) <= sizeof(size_t), 
			ValueType, ValueType&>::type ValueTypeParam;
		typedef typename ConditinalType< sizeof(KeyType) <= sizeof(size_t), 
			const KeyType, const KeyType&>::type KeyTypeConstParam;
		typedef typename ConditinalType< sizeof(ValueType) <= sizeof(size_t), 
			const ValueType, const ValueType&>::type ValueTypeConstParam;
		typedef KeyType& KeyTypeRef;
		typedef ValueType& ValueTypeRef;

		struct MapNode;
		class Iterator
		{
			friend ArrayMap;
			ArrayMap* _owner;
			uint32 _idx;

		public:
			Iterator(const uint32 idx, ArrayMap* owner) :
				_idx(idx),
				_owner(owner)
			{ }

			Iterator operator++()
			{
				_ASSERT( _idx < _owner->_data.size() );
				++_idx;
				return *this;
			}

			Iterator operator++(int)
			{
				_ASSERT( _idx < _owner->_data.size() );
				++_idx;
				return *this;
			}

			Iterator operator--()
			{
				_ASSERT( _idx > 0 );
				--_idx;
				return *this;
			}

			Iterator operator--(int)
			{
				_ASSERT( _idx > 0 );
				--_idx;
				return *this;
			}

			bool operator!=(const Iterator& other) const
			{
				return _idx != other._idx;
			}

			bool operator==(const Iterator& other) const
			{
				return _idx == other._idx;
			}

			const MapNode* operator->()  const
			{
				_ASSERT( _idx < _owner->_data.size() );
				return &_owner->_data[_idx];
			}

			MapNode* operator->()
			{
				_ASSERT( _idx < _owner->_data.size() );
				return &_owner->_data[_idx];
			}

			const MapNode& operator*() const
			{
				_ASSERT( _idx < _owner->_data.size() );
				return _owner->_data[_idx];
			}

			MapNode& operator*() 
			{
				_ASSERT( _idx < _owner->_data.size() );
				return _owner->_data[_idx];
			}
		};

		typedef Iterator iterator;
		friend Iterator;

	private:
		struct MapNode
		{
		private:
			friend ArrayMap;
			friend ArrayMap::Iterator;

		public:
			KeyType Key;
			ValueType Value;

			MapNode(KeyTypeConstParam k, ValueTypeConstParam v) :
				Key(k), Value(v)
			{ }

			MapNode(KeyTypeConstParam k) :
				Key(k)
			{ }
		};

	private:
		Array<MapNode> _data;
		IsKeyEqual _equal;

	public:
		ArrayMap(uint32 baseSize = 0, IsKeyEqual equal = IsKeyEqual()) :
			_equal(equal)
		{
			if(baseSize > 0)
				_data.reserve(baseSize);
		}

		~ArrayMap()
		{

		}

		Iterator find(KeyTypeConstParam key)
		{
			for(uint32 i = 0; i < _data.size(); ++i)
			{
				if( _equal(_data[i].Key, key) )
					return Iterator( i, this );
			}
			return end();
		}

		ValueTypeRef operator[](KeyTypeConstParam key)
		{
			for(uint32 i = 0; i < _data.size(); ++i)
			{
				if( _equal(_data[i].Key, key) )
					return _data[i].Value;
			}

			// Add new value
			_data.push_back( MapNode(key) );
			return _data[_data.size()-1].Value;
		}

		void insert(KeyTypeConstParam key, ValueTypeConstParam val)
		{
			(*this)[key] = val;
		}

		void insert(const std::pair<KeyType, ValueType>& nodePair)
		{
			(*this)[nodePair.first] = nodePair.second;
		}

		// Removes element from map and returns next element iterator
		Iterator erase(const Iterator& position)
		{
			_ASSERT( isNotEnd(position) );
			uint32 pos = position._idx;
			removeAt(pos);
			if( pos >= _data.size() )
				return Iterator(_data.size(), this);
			else
				return Iterator(pos, this);
		}

		// Removes element from map
		void erase(KeyTypeConstParam key)
		{
			uint32 node = 0;
			for(; node < _data.size(); ++node)
			{
				if( _equal(_data[node].Key, key) )
					break;
			}

			if( node == _data.size() )
				return; // There was no such element

			removeAt(node);
		}

		Iterator begin()
		{
			return Iterator(0, this);
		}

		Iterator end()
		{
			return Iterator(_data.size(), this);
		}

		bool isEnd(const Iterator& it) const
		{
			return it._idx >= _data.size();
		}

		bool isNotEnd(const Iterator& it) const
		{
			return it._idx < _data.size();
		}

		uint32 size() const
		{
			return _data.size();
		}

		void clear()
		{
			_data.clear();
		}

		void removeAt(uint32 idx)
		{
			for(uint32 i = idx; i < _data.size() - 1; ++i)
			{
				_data[i] = _data[i+1];
			}
			_data.pop_back();
		}
	};

	template<typename ValT>
	struct ArrayMaps
	{
		typedef ArrayMap<string, ValT, StringIsEqual> KeyString;
		typedef ArrayMap<char*, ValT, StringIsEqual> KeyCString;
		typedef ArrayMap<const char*, ValT, StringIsEqual> KeyCCString;
	};
}