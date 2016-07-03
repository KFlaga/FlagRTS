#pragma once

namespace FlagRTS
{
	template<typename NodeType, typename ContainerType>
	class GenericConstIterator
	{
	protected:
		friend ContainerType;
		const NodeType* _node;

		GenericConstIterator(const NodeType* node) :
			_node(node)
		{ }

	public:
		GenericConstIterator() :
			_node(0)
		{ }

		bool operator!=(const GenericConstIterator& other) const
		{
			return _node != other._node;
		}

		bool operator==(const GenericConstIterator& other) const
		{
			return _node == other._node;
		}

		const NodeType* operator->() const
		{
			_ASSERT( _node != 0 );
			return _node;
		}

		const NodeType& operator*() const
		{
			_ASSERT( _node != 0 );
			return *_node;
		}

		const NodeType& GetNode() const
		{
			_ASSERT( _node != 0 );
			return *_node;
		}
	};

	// Iterators over single or double linked lists
	template<typename NodeType, typename ContainerType>
	class GenericIterator
	{
	protected:
		friend ContainerType;
		NodeType* _node;

		GenericIterator(NodeType* node) :
			_node(node)
		{ }

	public:
		GenericIterator() :
			_node(0)
		{ }

		bool operator!=(const GenericIterator& other) const
		{
			return _node != other._node;
		}

		bool operator==(const GenericIterator& other) const
		{
			return _node == other._node;
		}

		NodeType* operator->() const
		{
			_ASSERT( _node != 0 );
			return _node;
		}

		const NodeType& operator*() const
		{
			_ASSERT( _node != 0 );
			return *_node;
		}

		NodeType& operator*()
		{
			_ASSERT( _node != 0 );
			return *_node;
		}

		NodeType& GetNode()
		{
			_ASSERT( _node != 0 );
			return *_node;
		}

		const NodeType& GetNode() const
		{
			_ASSERT( _node != 0 );
			return *_node;
		}

		operator GenericConstIterator<NodeType, ContainerType>() const
		{
			return GenericConstIterator<NodeType, ContainerType>(_node);
		}
	};

	template<typename NodeType, typename ContainerType>
	bool operator==(const GenericConstIterator<NodeType, ContainerType>& it1, 
		const GenericIterator<NodeType, ContainerType>& it2)
	{
		return it1._node == it2._node;
	}

	template<typename NodeType, typename ContainerType>
	bool operator==(const GenericIterator<NodeType, ContainerType>& it1, 
		const GenericConstIterator<NodeType, ContainerType>& it2)
	{
		return it1._node == it2._node;
	}

	template<typename NodeType, typename ContainerType>
	class ForwardConstIterator : public GenericConstIterator<NodeType, ContainerType>
	{
	private:
		friend ContainerType;

		ForwardConstIterator(const NodeType* node) :
			GenericConstIterator(node)
		{ }

	public:
		ForwardConstIterator() :
			GenericConstIterator()
		{ }

		ForwardConstIterator operator++()
		{
			_ASSERT( _node != 0 );
			ContainerType::Next(*this);
			return *this;
		}

		ForwardConstIterator operator++(int)
		{
			_ASSERT( _node != 0 );
			ContainerType::Next(*this);
			return *this;
		}
	};

	template<typename NodeType, typename ContainerType>
	class BiConstIterator : public GenericConstIterator<NodeType, ContainerType>
	{
	private:
		friend ContainerType;

		BiConstIterator(const NodeType* node) :
			GenericConstIterator(node)
		{ }

	public:
		BiConstIterator() :
			GenericConstIterator()
		{ }

		BiConstIterator operator++()
		{
			_ASSERT( _node != 0 );
			ContainerType::Next(*this);
			return *this;
		}

		BiConstIterator operator++(int)
		{
			_ASSERT( _node != 0 );
			ContainerType::Next(*this);
			return *this;
		}

		BiConstIterator operator--()
		{
			_ASSERT( _node != 0 );
			ContainerType::Prev(*this);
			return *this;
		}

		BiConstIterator operator--(int)
		{
			_ASSERT( _node != 0 );
			ContainerType::Prev(*this);
			return *this;
		}
	};

	template<typename NodeType, typename ContainerType>
	class ForwardIterator : public GenericIterator<NodeType, ContainerType>
	{
	private:
		friend ContainerType;

		ForwardIterator(NodeType* node) :
			GenericIterator(node)
		{ }

	public:
		ForwardIterator() :
			GenericIterator()
		{ }

		ForwardIterator operator++()
		{
			_ASSERT( _node != 0 );
			ContainerType::Next(*this);
			return *this;
		}

		ForwardIterator operator++(int)
		{
			_ASSERT( _node != 0 );
			ContainerType::Next(*this);
			return *this;
		}

		operator ForwardConstIterator<NodeType, ContainerType>() const
		{
			return ForwardConstIterator<NodeType, ContainerType>(_node);
		}
	};

	template<typename NodeType, typename ContainerType>
	class BiIterator : public GenericIterator<NodeType, ContainerType>
	{
	private:
		friend ContainerType;

		BiIterator(NodeType* node) :
			GenericIterator(node)
		{ }

	public:
		BiIterator() :
			GenericIterator()
		{ }

		BiIterator operator++()
		{
			_ASSERT( _node != 0 );
			ContainerType::Next(*this);
			return *this;
		}

		BiIterator operator++(int)
		{
			_ASSERT( _node != 0 );
			ContainerType::Next(*this);
			return *this;
		}

		BiIterator operator--()
		{
			_ASSERT( _node != 0 );
			ContainerType::Prev(*this);
			return *this;
		}

		BiIterator operator--(int)
		{
			_ASSERT( _node != 0 );
			ContainerType::Prev(*this);
			return *this;
		}

		operator BiConstIterator<NodeType, ContainerType>() const
		{
			return BiConstIterator<NodeType, ContainerType>(_node);
		}
	};
}