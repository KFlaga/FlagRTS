#pragma once

#include "TypeDefs.h"
#include "ObjectPoolHandle.h"
#include "ObjectNode.h"
#include <map>

namespace FlagRTS
{
	class ObjectsPool
	{
	protected:
		uint16 _nodeSize; // Items in single node - same thing as pool increment
		uint32 _itemsCount; // Number of items allocated in all nodes
		size_t _objectSize;

		typedef std::map<uint16, ObjectNode*> NodeMap;
		NodeMap _nodes;
		ObjectNode* _firstFreeNode; // First node on list with available free items

	public:
		ObjectsPool(size_t objectSize, uint16 poolIncrement);

		~ObjectsPool();

		uint16 GetPoolIncrement() const { return _nodeSize; }
		uint16 GetNodesCount() const { return _nodes.size(); }

		MemoryChunk* GetNewItem() 
		{
			_itemsCount++;
			return _firstFreeNode->GetNewItem();
		}

		bool FreeItem(MemoryChunk*); // Frees item in one of the nodes

		typedef LambdaDelegate< void(*)(ObjectsPool*, ObjectNode*), ObjectsPool*, ObjectNode*> ObjectPoolDelegate;
		void ForEach(ObjectPoolDelegate& callback);

		size_t GetFreeMemory()
		{
			return (GetNodesCount() * _nodeSize - _itemsCount) * _objectSize;
		}

		size_t GetUsedMemory() const
		{
			return _itemsCount * _objectSize;
		}

		size_t GetReservedMemory() const
		{
			return GetNodesCount() * _nodeSize * _objectSize;
		}

		void Reserve(size_t size)
		{
			while(GetFreeMemory() < size)
			{
				CreateNode();
			}
		}

	protected:
		uint16 GetNewNodeId() { return _nodes.size(); }

		ObjectNode* CreateNode();

		void ReleaseNode(ObjectNode* node)
		{
			_nodes.erase(node->GetId());
		}

		ObjectNode* FindNode(uint16 nodeId) 
		{ 
			auto it = _nodes.find(nodeId);
			if(it != _nodes.end())
				return it->second;
			else
				return 0;
		}

		// Callback functions to sygnalize node state changes

		class ObjectPoolEventHandler : public EventHandler<ObjectNode*>
		{
		protected:
			ObjectsPool* _pool;
		public:
			ObjectPoolEventHandler(ObjectsPool* pool) : _pool(pool) { }
		};
		
		void OnNodeFull(ObjectNode* node);
		void OnNodeFreed(ObjectNode* node);
		void OnNodeEmpty(ObjectNode* node);

		class CBOnNodeFull : public ObjectPoolEventHandler
		{
		public:
			CBOnNodeFull(ObjectsPool* pool) : ObjectPoolEventHandler(pool) { }
			void operator()(ObjectNode* node) { _pool->OnNodeFull(node); }
		} _cbOnNodeFull;

		class CBOnNodeFreed : public ObjectPoolEventHandler
		{
		public:
			CBOnNodeFreed(ObjectsPool* pool) : ObjectPoolEventHandler(pool) { }
			void operator()(ObjectNode* node) { _pool->OnNodeFreed(node); }
		} _cbOnNodeFreed;

		class CBOnNodeEmpty : public ObjectPoolEventHandler
		{
		public:
			CBOnNodeEmpty(ObjectsPool* pool) : ObjectPoolEventHandler(pool) { }	
			void operator()(ObjectNode* node) { _pool->OnNodeEmpty(node); }
		} _cbOnNodeEmpty;
	};
}