#pragma once

#include "ObjectNode.h"

namespace FlagRTS
{
	
	ObjectNode::ObjectNode(size_t itemSize, uint16 itemCount, uint16 id) : 
		_itemSize(itemSize),
		_totalBlockCount(itemCount),
		_freeBlocksCount(itemCount),
		_firstAlloc(0),
		_lastAlloc(0),
		_nodeId(id)
	{
		// Alloc memory for MemItems and for data stored in them
		_dataMemoryBegin = reinterpret_cast<char*>(std::malloc((_itemSize+sizeof(MemoryChunk))*itemCount));
		_itemsMemoryBegin = new MemItem[itemCount];

		// Assing next MemItems from table
		// First one have no precedessor
		// Assing data to it from table
		MemItem* curItem = _itemsMemoryBegin;
		curItem->Prev = 0;
		curItem->Index = 0;
		_firstFree = curItem;

		// Alloc memory for MemItems and for data stored in them
		_itemsMemoryBegin = new MemItem[itemCount];

		for (int item = 1; item < itemCount; item++)
		{
			// Every next MemItem is on next position in table
			curItem->Next = &(_itemsMemoryBegin[item]);
			// Next MemItem will have a current one as precedessor
			curItem->Next->Prev = curItem;
			// Current one is done, so assing next one to curItem
			curItem = curItem->Next;
			curItem->Index = item;
		}

		_lastFree = curItem;
		curItem->Next = 0;
	}

	ObjectNode::~ObjectNode()
	{
		std::free(_dataMemoryBegin);
		delete[] (_itemsMemoryBegin);
	}

	MemoryChunk* ObjectNode::GetNewItem()
	{
		// Get first free item
		_freeBlocksCount--;
		MemItem* item = _firstFree;
		// Set new first free
		if (item != _lastFree)
		{
			item->Next->Prev = 0;
			_firstFree = item->Next;
		}
		// Put item in alloc list
		item->Prev = _lastAlloc;
		if (_lastAlloc != 0)
			_lastAlloc->Next = item;
		else
			_firstAlloc = item;

		_lastAlloc = item;
		item->Next = 0;

		if (_freeBlocksCount == 0)
			OnNodeFull(this);

		MemoryChunk* mem = GetDataOnPosition(item->Index);
		mem->Header.info = ObjectPoolHandle(_nodeId, item->Index);
		mem->Header.size = _itemSize;
		return mem;
	}

	bool ObjectNode::FreeItem(ObjectPoolHandle handle)
	{
		if (handle.GetItemIdx() > _totalBlockCount) // Out of range
			return false;

		MemItem* item = &_itemsMemoryBegin[handle.GetItemIdx()];
		if (item->IsFree)
			return false;

		// If found allocated item move it from allocList to freeList
		if (item != _lastAlloc)
			item->Next->Prev = item->Prev;
		if (item != _firstAlloc)
			item->Prev->Next = item->Next;

		if (_lastFree != 0)
		{
			_lastFree->Next = item;
			item->Prev = _lastFree;
		}
		else
			item->Prev = 0;
		item->Next = 0;
		_freeBlocksCount++;

		if (_freeBlocksCount == 1)
			OnNodeFreed.Fire(this);
		else if (_freeBlocksCount == _totalBlockCount)
			OnNodeEmpty.Fire(this);

		return true;
	}

	void ObjectNode::ForEach(ObjectNodeDelegate& callback)
	{
		MemItem* item = _firstAlloc;
		while (item != 0)
		{
			callback(this, reinterpret_cast<MemoryChunk*>(_dataMemoryBegin[item->Index]));
			item = item->Next;
		}
	}
}