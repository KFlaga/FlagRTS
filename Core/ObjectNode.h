#pragma once

#include "TypeDefs.h"
#include "Event.h"
#include "MemoryChunk.h"
#include "ObjectPoolHandle.h"
#include "Delegates.h"
#include <stdlib.h>

namespace FlagRTS
{
	// Object nodes contains set amount of object of template type
	class ObjectNode
	{
	private:
		
	protected:
		class MemItem // Simple class to create linked list of items
		{
		public:
			MemItem* Next;
			MemItem* Prev;
			bool IsFree;
			uint16 Index;
		};

	protected:
		size_t _itemSize; // Size of single object
		uint16 _nodeId; // Idetyficator of this node
		uint16 _totalBlockCount; // Total number of items in node
		uint16 _freeBlocksCount; // Remaining free items

		char* _dataMemoryBegin; // Address where memory for items' data begins
		MemItem* _itemsMemoryBegin; // Address where memory for MemItems begins

		MemItem* _firstFree; // Free MemItem list
		MemItem* _lastFree;
		MemItem* _firstAlloc; // Allocated MemItem list
		MemItem* _lastAlloc;

	public:
		ObjectNode(size_t itemSize, uint16 itemCount, uint16 nodeId);
		~ObjectNode();

		uint16 ItemsCount() const { return _totalBlockCount; }
		uint16 FreeItemsCount() const { return _freeBlocksCount; }
		bool IsFreeItemsAvailable() const { return (_freeBlocksCount > 0); }

		char* GetDataMemoryBegin() const { return _dataMemoryBegin; }
		uint16 GetId() const { return _nodeId; }
		uint16 GetItemSize() const { return _nodeId; }

		MemoryChunk* GetNewItem(); // Saves address of next free item

		bool FreeItem(ObjectPoolHandle handle); // Frees address, return false on failure

		MemoryChunk* GetDataOnPosition(uint16 idx)
		{
			return reinterpret_cast<MemoryChunk*>(_dataMemoryBegin + (_itemSize+sizeof(MemoryChunk))*idx);
		}

		Event<ObjectNode*> OnNodeFull; // Called when freeCount == 0
		Event<ObjectNode*> OnNodeFreed; // Called when freeCount changes from 0 to 1
		Event<ObjectNode*> OnNodeEmpty; // Called when freeCount changes to totalCount

		typedef LambdaDelegate<void(*)(ObjectNode*, MemoryChunk*),ObjectNode*, MemoryChunk*> ObjectNodeDelegate;
		void ForEach(ObjectNodeDelegate&);	
	};
}