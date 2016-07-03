#pragma once

#include "TypeDefs.h"
#include "Array.h"
#include "Iterators.h"

#ifdef MAP_DEBUG_NEW
#define mapNew0 xNew0
#define mapNew1 xNew1
#define mapNew2 xNew2
#define mapNew3 xNew3
#define mapDelete xDelete
#else
#define mapNew0 ndNew0
#define mapNew1 ndNew1
#define mapNew2 ndNew2
#define mapNew3 ndNew3
#define mapDelete ndDelete
#endif

namespace FlagRTS
{
	// TODO Reorganize maps buckets on other thread with low priority when we'll have spare time
	// (option via HashMapBase)
	class HashMapBase
	{
	protected:
		uint32 _bucketsCount;
		uint32 _shift;

	public:
		HashMapBase() { }
		virtual ~HashMapBase() { }

		// Default version for intergers only
		struct HashInteger
		{
			uint32 operator()(uint32 a)
			{
				/*a = (a ^ 61) ^ (a >> 16);
				a = a + (a << 3);
				a = a ^ (a >> 4);
				a = a * 0x27d4eb2d;
				a = a ^ (a >> 15);
				return a;*/

				a += ~(a<<15);
				a ^=  (a>>10);
				a +=  (a<<3);
				a ^=  (a>>6);
				a += ~(a<<11);
				a ^=  (a>>16);
				return a;
			}
		};

		struct HashString
		{
			static const uint32 HashConst = 5381;
			uint32 operator()(const char* str)
			{
				uint32 hash = HashConst;
				int c;

				while (c = *str++)
					hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

				return hash;
			}

			uint32 operator()(const string& str)
			{
				return str.size() < 16 ?
					operator()(str.c_str()) :
				HashLongOne(str);
			}

			uint32 HashLongOne(const string& str)
			{
				uint32 hash = HashConst;
				uint32 skip = (str.size() >> 4) + 1; // 1/16 size of string + 1

				for(uint32 pos = 0; pos < str.size(); pos += skip)
					hash = ((hash << 5) + hash) + (uint32)str[pos]; /* hash * 33 + c */

				return hash;
			}
		};
	};

	template<typename KeyT, typename ValT, typename Hash = HashMapBase::HashInteger, typename IsKeyEqual = std::equal_to<KeyT>>
	class HashedMap : public HashMapBase
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
		typedef ForwardIterator<MapNode, HashedMap> Iterator;
		typedef Iterator iterator;
		friend Iterator;

	private:
		struct MapNode
		{
		private:
			MapNode* Prev;
			MapNode* Next;
			friend HashedMap;
			friend HashedMap::Iterator;

		public:
			KeyType Key;
			ValueType Value;

			MapNode(KeyTypeConstParam k, ValueTypeConstParam v, MapNode* next, MapNode* prev) :
				Key(k), Value(v), Next(next), Prev(prev)
			{ }

			MapNode(KeyTypeConstParam k, MapNode* next, MapNode* prev) :
				Key(k), Next(next), Prev(prev)
			{ }

			MapNode(KeyTypeConstParam k) :
				Key(k)
			{ }

			MapNode(KeyTypeConstParam k, ValueTypeConstParam v) :
				Key(k), Value(v)
			{ }
		};

	private:
		// Bucket is array of pointer to Key-Val nodes
		// Nodes create single linked lists ( in order in each bucket, but buckets themselves can be in arbitrary order )
		typedef Array<MapNode*> Bucket;
		Array<Bucket> _buckets; 
		Hash _hasher;
		IsKeyEqual _equal;
		MapNode* _begin;
		MapNode* _end;
		uint32 _totalNodes;
		uint32 _baseBucketSize;
		uint8 _baseShift;

	public:
		HashedMap(uint8 baseShift = 10u, uint32 baseBucketSize = 4u, Hash hasher = Hash(), IsKeyEqual equal = IsKeyEqual()) :
			_baseShift(baseShift),
			_baseBucketSize(baseBucketSize),
			_totalNodes(0), 
			_begin(0),
			_end(0), 
			_hasher(hasher),
			_equal(equal)
		{
			_shift = _baseShift;
			_bucketsCount = 1 << baseShift;
			Array<MapNode*> baseArray;
			baseArray.reserve(baseBucketSize);
			_buckets.resize( _bucketsCount, baseArray );
		}

		~HashedMap()
		{
			for(auto it = begin(); isNotEnd(it);)
			{
				auto node = it._node;
				++it;
				mapDelete( node );
			}
		}

		uint32 getHashed(KeyTypeConstParam key)
		{
			uint32 hash = _hasher(key);
			return hash - ((hash >> _shift) << _shift);
		}

		Iterator find(KeyTypeConstParam key)
		{
			Bucket& bucket = getBucket(key);
			uint32 node = 0u;
			for(; node < bucket.size(); ++node)
			{
				//if (_allNodes[bucket[node]].Key == key)
				//	return Iterator(this, bucket[node]);
				if( _equal(bucket[node]->Key, key) )
					return Iterator(bucket[node]);
			}
			return end();
		}

		ValueTypeRef operator[](KeyTypeConstParam key)
		{
			Bucket& bucket = getBucket(key);
			if( _totalNodes == 0)
			{
				return AddFirstToMap(bucket, key)->Value;
			}

			if( bucket.size() == 0 )
			{
				return AddFirstToBucket(bucket, key)->Value;
			}

			uint32 node = 0u;
			for(; node < bucket.size(); ++node)
			{
				if(_equal(bucket[node]->Key, key))
					return bucket[node]->Value;
			}

			// Add new value
			MapNode* newNode = PushToBucket(bucket, key);

			if(_totalNodes * 0.75f > _bucketsCount)
			{
				RehashMap(_bucketsCount << 1 );
			}

			return newNode->Value;
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
			return Iterator(Erase(position->Key));
		}

		// Removes element from map and returns next element
		MapNode* erase(KeyTypeConstParam key)
		{
			Bucket& bucket = getBucket(key);
			if( _totalNodes == 1)
			{
				bucket.clear();
				mapDelete(_begin);
				_begin = 0;
				_end = 0;
				_totalNodes = 0;
				return 0;
			}

			uint32 node = 0;
			for(; node < bucket.size(); ++node)
			{
				if( _equal(bucket[node]->Key, key) )
					break;
			}

			if( node == bucket.size() )
				return 0; // There was no such element

			MapNode* toRemove = bucket[node];
			MapNode* next = toRemove->Next;
			if(toRemove == _begin)
			{
				// Begin needs to ne changed
				_begin = _begin->Next;
				_begin->Prev = 0;
			}
			else if(toRemove == _end)
			{
				_end = _end->Prev;
				_end->Next = 0;
			}
			else
			{
				toRemove->Prev->Next = toRemove->Next;
				if(toRemove->Prev != 0)
					toRemove->Next->Prev = toRemove->Prev;
			}

			// Move rest nodes from the bucket
			for(; node < bucket.size() - 1; ++node)
			{
				bucket[node] = bucket[node+1];
			}

			--_totalNodes;
			bucket.pop_back();
			mapDelete(toRemove);
			return next;
		}

		Iterator begin()
		{
			return Iterator(_begin);
		}

		Iterator end()
		{
			return Iterator(0);
		}

		bool isEnd(const Iterator& it) const
		{
			return it._node == 0;
		}

		bool isNotEnd(const Iterator& it) const
		{
			return it._node != 0;
		}

		uint32 size() const
		{
			return _totalNodes;
		}

		void clear()
		{
			for(auto it = begin(); isNotEnd(it); ++it)
			{
				auto node = it._node;
				++it;
				mapDelete( node );
			}

			_buckets.clear();
			Array<MapNode*> baseArray;
			baseArray.reserve(_baseBucketSize);
			_shift = _baseShift;
			_bucketsCount = 1 << _baseShift;
			_buckets.resize( _bucketsCount, baseArray );
		}

		Bucket& getBucket(KeyTypeConstParam key)
		{
			return _buckets[getHashed(key)];
		}

	protected:
		MapNode* AddFirstToMap(Bucket& bucket, KeyTypeConstParam key)
		{
			MapNode* newNode = mapNew3(MapNode, key, (MapNode*)0, (MapNode*)0); 
			_begin = newNode;
			_end = newNode;
			bucket.push_back(newNode);
			++_totalNodes;
			return newNode;
		}

		MapNode* AddFirstToBucket(Bucket& bucket, KeyTypeConstParam key)
		{
			MapNode* newNode = mapNew3(MapNode, key, (MapNode*)0, _end);
			_end->Next = newNode;
			_end = newNode;
			bucket.push_back(newNode);
			++_totalNodes;
			return newNode;
		}

		MapNode* PushToBucket(Bucket& bucket, KeyTypeConstParam key)
		{
			++_totalNodes;
			MapNode* lastInBucket = bucket.back();
			MapNode* newNode = mapNew3(MapNode, key, lastInBucket->Next, lastInBucket); 
			lastInBucket->Next = newNode;
			bucket.push_back(newNode);
			return newNode;
		}

		void RehashMap(size_t newBucketCount)
		{

		}

		static void Next(Iterator& it)
		{
			it._node = it._node->Next;
		}
	};

	template<typename ValT>
	struct HashedMaps
	{
		typedef HashedMap<string, ValT, HashMapBase::HashString, StringIsEqual> KeyString;
		typedef HashedMap<char*, ValT, HashMapBase::HashString, StringIsEqual> KeyCString;
		typedef HashedMap<const char*, ValT, HashMapBase::HashString, StringIsEqual> KeyCCString;
	};
}