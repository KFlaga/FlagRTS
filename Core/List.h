#pragma once

#include "TypeDefs.h"
#include "AllocatorStdWrapper.h"
#include "Iterators.h"

#ifdef LIST_DEBUG_NEW
#define listNew0 xNew0
#define listNew1 xNew1
#define listNew2 xNew2
#define listNew2 xNew3
#define listDelete xDelete
#else
#define listNew0 ndNew0
#define listNew1 ndNew1
#define listNew2 ndNew2
#define listNew3 ndNew3
#define listDelete ndDelete
#endif

namespace FlagRTS
{
	// Generic double linked list with partialy STL like interface
	template<typename ValT>
	class List
	{
	public:
		typedef ValT ValueType;
		typedef typename ConditinalType< sizeof(ValueType) <= sizeof(size_t), 
			ValueType, ValueType&>::type ValueTypeParam;
		typedef const ValueTypeParam ValueTypeConstParam;
		typedef ValueType& ValueTypeRef;

		struct ListNode
		{
		private:
			ListNode* Prev;
			ListNode* Next;
			friend List;

		public:
			ValueType Value;

			ListNode(ValueTypeConstParam v, ListNode* next, ListNode* prev) :
				Value(v), Next(next), Prev(prev)
			{ }

			ListNode(ListNode* next, ListNode* prev) :
				Next(next), Prev(prev)
			{ }

			ListNode() :
				Prev(0),
				Next(0)
			{ }

			ListNode(ValueTypeConstParam v) :
				Value(v),
				Prev(0),
				Next(0)
			{ }

			operator ValueType&()
			{
				return Value;
			}

			operator const ValueType&() const
			{
				return Value;
			}
		};

		typedef BiIterator<ListNode, List> Iterator;
		typedef Iterator iterator;
		typedef BiConstIterator<ListNode, List> ConstIterator;
		typedef ConstIterator const_iterator;
		friend Iterator;
		friend ConstIterator;

	private:
		ListNode* _begin;
		ListNode* _end;
		uint32 _totalNodes;
		ListNode _outOfBounds;

	public:
		List() :
			_totalNodes(0),
			_outOfBounds(0, 0),
			_begin(&_outOfBounds),
			_end(&_outOfBounds)
		{ }

		List(const List& other) :
			_outOfBounds(0, 0),
			_totalNodes(0),
			_begin(&_outOfBounds),
			_end(&_outOfBounds)
		{
			copyFrom(other);
		}

		List& operator=(const List& other)
		{
			copyFrom(other);
			return *this;
		}

		List(size_t n) :
			_totalNodes(n)
		{
			_ASSERT( n == 0 ); // Just do not use it for n = 0

			_begin = listNew0(ListNode);
			ListNode* node = _begin;
			ListNode* nextNode = 0;
			for(size_t i; i < n; ++n)
			{
				nextNode = listNew0(ListNode);
				nextNode->Prev = node;
				node->Next = nextNode;
				node = nextNode;
			}
			_end = node;
			_end->Next = &_outOfBounds;
			_outOfBounds.Prev = _end;
		}

		List(size_t n, ValueTypeConstParam val) :
			_totalNodes(n)
		{
			_ASSERT( n == 0 ); // Just do not use it for n = 0

			_begin = listNew1(ListNode, val);
			ListNode* node = _begin;
			ListNode* nextNode = 0;
			for(size_t i; i < n; ++n)
			{
				nextNode = listNew1(ListNode, val);
				nextNode->Prev = node;
				node->Next = nextNode;
				node = nextNode;
			}
			_end = node;
			_end->Next = &_outOfBounds;
			_outOfBounds.Prev = _end;
		}

		~List()
		{
			clear();
		}

		uint32 size() const
		{
			return _totalNodes;
		}

		void insert(Iterator insertAfterThis, ValueTypeConstParam val)
		{
			_ASSERT(insertAfterThis._node != &_outOfBounds); 
			_ASSERT(insertAfterThis._node != 0 );

			ListNode* node = listNew3(ListNode, val, insertAfterThis->Next, insertAfterThis->Prev);
			insertAfterThis->Next->Prev = node;
			insertAfterThis->Next = node;

			++_totalNodes;
		}

		Iterator erase(Iterator eraseThis)
		{
			_ASSERT(eraseThis._node != &_outOfBounds); 
			_ASSERT(eraseThis._node != 0 );
			_ASSERT(_totalNodes > 0 );

			ListNode* oldNode = eraseThis._node;
			Iterator next = Iterator(oldNode->Next);
			if( _totalNodes == 1 )
			{
				_begin = &_outOfBounds;
				_end = &_outOfBounds;
				_outOfBounds.Prev = 0;
			}
			else if( oldNode == _begin )
			{
				_begin = _begin->Next;
				_begin->Prev = 0;
			}
			else if( oldNode == _end )
			{
				_end = oldNode->Prev;
				_end->Next = &_outOfBounds;
			}
			else
			{
				oldNode->Prev->Next = oldNode->Next;
				oldNode->Next->Prev = oldNode->Prev;
			}
			listDelete(oldNode);
			--_totalNodes;

			return next;
		}

		void push_back(ValueTypeConstParam val)
		{
			if( _totalNodes == 0)
			{
				InsertFirst(val);
				return;
			}

			ListNode* node = listNew3(ListNode, val, &_outOfBounds, _end);
			_end->Next = node;
			_end = node;
			_outOfBounds.Prev = _end;
			++_totalNodes;
		}

		void pop_back()
		{
			_ASSERT( _totalNodes > 0 );
			if( _totalNodes == 1)
			{
				RemoveLast();
				return;
			}

			ListNode* oldEnd = _end;
			_end = _end->Prev;
			_end->Next = &_outOfBounds;
			_outOfBounds.Prev = _end;
			listDelete(oldEnd);
			--_totalNodes;
		}

		void push_front(ValueTypeConstParam val)
		{
			if( _totalNodes == 0)
			{
				InsertFirst(val);
				return;
			}

			ListNode* node = listNew3(ListNode, val, _begin, (ListNode*)0);
			_begin->Prev = node;
			_begin = node;
			++_totalNodes;
		}

		void pop_front()
		{
			_ASSERT( _totalNodes > 0 );
			if( _totalNodes == 1)
			{
				RemoveLast();
				return;
			}

			ListNode* oldBegin = _begin;
			_begin = _begin->Next;
			_begin->Prev = 0;
			listDelete(oldBegin);
			--_totalNodes;
		}

		void clear()
		{
			if( _totalNodes == 0 )
				return;

			ListNode* node = _begin;
			ListNode* nextnode;
			while( node != &_outOfBounds )
			{
				nextnode = node->Next;
				listDelete(node);
				node = nextnode;
			}
			_totalNodes = 0;
			_begin = _end = &_outOfBounds;
			_outOfBounds.Next = _outOfBounds.Prev = 0;
		}

		// Copies contents of other list
		// Clears list first, allocates new nodes, copies values
		void copyFrom(const List& other)
		{
			clear();
			_totalNodes = other._totalNodes;
			if( _totalNodes > 0 )
			{
				ListNode* otherNode = other._begin;
				_begin = listNew1(ListNode, otherNode->Value);
				ListNode* node = _begin;
				ListNode* nextNode = 0;
				otherNode = otherNode->Next;
				while(otherNode != &other._outOfBounds)
				{
					nextNode = listNew1(ListNode, otherNode->Value);
					nextNode->Prev = node;
					node->Next = nextNode;
					node = nextNode;
					otherNode = otherNode->Next;
				}
				_end = node;
				_end->Next = &_outOfBounds;
				_outOfBounds.Prev = _end;
			}
		}

		// Appends contenes of other list
		// Allocates new nodes, copies values
		void merge(const List& other)
		{
			if( other._totalNodes > 0 )
			{
				ListNode* otherNode = other._begin;
				ListNode* node;
				if( _totalNodes == 0 )
				{
					_begin = listNew1(ListNode, otherNode->Value);
					node = _begin;
				}
				else
				{
					node = listNew3(ListNode, otherNode->Value, (ListNode*)0, _end);
					_end->Next = node;
				}

				ListNode* nextNode = 0;
				otherNode = otherNode->Next;
				while(otherNode != &other._outOfBounds)
				{
					nextNode = listNew1(ListNode, otherNode->Value);
					nextNode->Prev = node;
					node->Next = nextNode;
					node = nextNode;
					otherNode = otherNode->Next;
				}
				_end = node;
				_end->Next = &_outOfBounds;
				_outOfBounds.Prev = _end;
				_totalNodes += other._totalNodes;
			}
		}

		// Returns iteration to first element which fulfills given condition -> by default it checks for equality
		// Condition function must be binary functor/lambda that accepts ValueType on both sides.
		// Returns end() if no element passed check
		template<typename CondFunc = std::equal_to<ValueType>>
		Iterator find(ValueTypeConstParam val, CondFunc condition = CondFunc())
		{
			ListNode* node = _begin;
			while(node != &_outOfBounds &&
				condition(node->Value, val) == false) 
			{
				node = node->Next;
			}
			return Iterator(node);
		}

		// Returns iterator to first element which fulfills given condition
		// Condition function must be unary functor/lambda that accepts ValueType
		// Returns end() if no element passed check
		template<typename CondFunc>
		Iterator findIf(CondFunc condition = CondFunc())
		{
			ListNode* node = _begin;
			while(node != &_outOfBounds &&
				condition(node->Value) == false) 
			{
				node = node->Next;
			}
			return Iterator(node);
		}

		// Removes first element which fulfills given condition -> by default it checks for equality
		// Condition function must be binary functor/lambda that accepts ValueType on both sides
		template<typename CondFunc = std::equal_to<ValueType>>
		void remove(ValueTypeConstParam val, CondFunc condition = CondFunc())
		{
			Iterator it = find(val, condition);
			if(it != end())
			{
				erase(it);
			}
		}

		// Removes first element which fulfills given condition
		// Condition function must be unary lambda/functor that accepts ValueType
		template<typename CondFunc>
		void removeIf(CondFunc condition = CondFunc())
		{
			Iterator it = find(condition);
			if(it != end())
			{
				erase(it);
			}
		}

		// Executes given function for each node in list
		// Functor/lambda should have 
		template<typename ValueFunc>
		void ForEach(ValueFunc func = ValueFunc())
		{
			ListNode* node = _begin;
			while(node != &_outOfBounds)
			{
				ValueFunc(node->Value);
			}
		}

		Iterator begin()
		{
			return Iterator(_begin);
		}

		Iterator end()
		{
			return Iterator(&_outOfBounds);
		}

		ConstIterator cbegin() const
		{
			return ConstIterator(_begin);
		}

		ConstIterator cend() const
		{
			return ConstIterator(&_outOfBounds);
		}

		ValueTypeRef front()
		{
			_ASSERT( _begin != 0 && _begin != &_outOfBounds );
			return *_begin;
		}

		const ValueTypeRef front() const
		{
			_ASSERT( _begin != 0 && _begin != &_outOfBounds );
			return *_begin;
		}

		ValueTypeRef back()
		{
			_ASSERT( _end != 0 && _end != &_outOfBounds );
			return *_end;
		}

		const ValueTypeRef back() const
		{
			_ASSERT( _end != 0 && _end != &_outOfBounds );
			return *_end;
		}

		bool isEnd(const Iterator it) const
		{
			return it._node == &_outOfBounds;
		}

		bool isEnd(const ConstIterator it) const
		{
			return it._node == &_outOfBounds;
		}
	protected:
		void InsertFirst(ValueTypeConstParam val)
		{
			ListNode* node = listNew3(ListNode, val, &_outOfBounds, (ListNode*)0);
			_begin = node;
			_end = node;
			_outOfBounds.Prev = _end;
			++_totalNodes;
		}

		void RemoveLast()
		{
			ListNode* old = _begin;
			_begin = &_outOfBounds;
			_end = &_outOfBounds;
			_outOfBounds.Prev = 0;
			listDelete(old);
			--_totalNodes;
		}

		static void Next(Iterator& it)
		{
			_ASSERT( it._node->Next != 0 );
			it._node = it._node->Next;
		}

		static void Prev(Iterator& it)
		{
			_ASSERT( it._node->Prev != 0 );
			it._node = it._node->Prev;
		}

		static void Next(ConstIterator& it)
		{
			_ASSERT( it._node->Next != 0 );
			it._node = it._node->Next;
		}

		static void Prev(ConstIterator& it)
		{
			_ASSERT( it._node->Prev != 0 );
			it._node = it._node->Prev;
		}
	};
}

#undef listNew0
#undef listNew1
#undef listNew2
#undef listNew3
#undef listDelete