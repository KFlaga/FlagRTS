#include "ObjectPool.h"

namespace FlagRTS
{
	ObjectsPool::ObjectsPool(size_t objectSize, uint16 poolIncrement) :
			_objectSize(objectSize),
			_nodeSize(poolIncrement),
			_itemsCount(0),
			_cbOnNodeEmpty(this),
			_cbOnNodeFreed(this),
			_cbOnNodeFull(this)
		{
			// Create first node
			_firstFreeNode = CreateNode();
		}	

		ObjectsPool::~ObjectsPool()
		{
			for(auto nodeIt = _nodes.begin(); nodeIt != _nodes.end(); ++nodeIt)
			{
				delete nodeIt->second;
			}
			_nodes.clear();
		}

	ObjectNode* ObjectsPool::CreateNode()
	{
		uint16 newid = GetNewNodeId();
		// Create node and insert on the end of list
		ObjectNode* node = new ObjectNode(_objectSize, _nodeSize, newid);
		_nodes[newid] = node;

		// Set callbacks
		node->OnNodeFull += &_cbOnNodeFull;
		node->OnNodeFreed += &_cbOnNodeFreed;
		node->OnNodeEmpty += &_cbOnNodeEmpty;

		return node;
	}

	bool ObjectsPool::FreeItem(MemoryChunk* mem)
	{
		ObjectPoolHandle handle(mem);

		auto it = _nodes.find(handle.GetNodeIdx());
		if(it == _nodes.end())
			return false;
		ObjectNode* node(it->second);

		// Free item in node and release the handle
		node->FreeItem(handle);
		mem->Header.info = INVALID_INDEX_32;
		_itemsCount--;

		return true;
	}

	void ObjectsPool::OnNodeFull(ObjectNode*)
	{
		// Find next free node
		// If there is no one, create one
		ObjectNode* node = 0;
		for(auto nodeIt = _nodes.begin(); nodeIt != _nodes.end(); ++nodeIt )
		{
			if (nodeIt->second->IsFreeItemsAvailable())
			{
				node = nodeIt->second;
				break;
			}
		}

		if (node == 0)
		{
			_firstFreeNode = CreateNode();
		}
		else
		{
			_firstFreeNode = node;
		}
	}

	void ObjectsPool::OnNodeFreed(ObjectNode* node)
	{
		// Find if it's idx is lower than firstFree
		// if is, set node as first free  - nope
	}

	void ObjectsPool::OnNodeEmpty(ObjectNode* node)
	{
		// If node is empty and there are at least _nodeSize
		// empty slots in other nodes, delete node
		if (_itemsCount < (GetNodesCount() - 2u)*_nodeSize)
		{
			// If node was first free node and we want to remove it, change free node
			if (_firstFreeNode == node)
			{
				ReleaseNode(node);
				for(auto nodeIt = _nodes.begin(); nodeIt != _nodes.end(); ++nodeIt )
				{
					if (nodeIt->second->IsFreeItemsAvailable())
					{
						_firstFreeNode = nodeIt->second;
						return;
					}
				}
			}
			else
				ReleaseNode(node);	
		}
	}

	void ObjectsPool::ForEach(ObjectPoolDelegate& callback)
	{
		for(auto it = _nodes.begin(); it != _nodes.end(); ++it)
		{
			callback(this,it->second);
		}
	}
}