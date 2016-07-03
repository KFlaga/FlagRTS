#pragma once

#include <Ogre\OgreVector3.h>
#include <Array.h>

namespace Ogre
{
	class VertexData;
}

namespace FlagRTS
{
	// Simple buffer of Vector3, representing positions of vertices of mesh
	class VertexBuffer
	{
	private:
		typedef Array<Ogre::Vector3> Buffer;
		Buffer _buffer;
		unsigned int _size;
		unsigned int _reserved;

	public:
		VertexBuffer();
		~VertexBuffer();

		// Clears buffer and sets buffer based on given VertexData
		void FromVertexData(Ogre::VertexData* vertexData);

		// Clears buffer and sets buffer based on given data
		// ( stores copy of data )
		void SetData(Ogre::Vector3* buffer, const unsigned int count);
		
		// Clears buffer and sets buffer based on given data
		// ( stores copy of data )
		void SetData(const Buffer& buffer, const unsigned int count);
		
		// Adds to buffer vertex from given VertexData, allocating new
		// buffer if there is not enough reserved memory
		void AddFromVertexData(Ogre::VertexData* vertexData);

		// Adds to buffer vertex from given data, allocating new
		// buffer if there is not enough reserved memory
		// ( stores copy of data )
		void AddData(const Buffer& buffer, const unsigned int count);
		
		// Adds to buffer vertex from given data, allocating new
		// buffer if there is not enough reserved memory
		// ( stores copy of data )
		void AddData(Ogre::Vector3* buffer, const unsigned int count);

		// Adds single vertex to buffer
		void AddVertex(const Ogre::Vector3& vertex);

		// Adds single vertex, dont check for available space ( assume it is reserved )
		void AddVertexUnsafe(const Ogre::Vector3& vertex);

		unsigned int GetVertexCount() const { return _size; }
		const Buffer& GetData() const { return _buffer; }

		const Ogre::Vector3& GetVertex(const unsigned int num) const 
		{
			return _buffer[num];
		}

		Ogre::Vector3& GetVertex(const unsigned int num) 
		{
			return _buffer[num];
		}

		const Ogre::Vector3& operator[](const unsigned int num) const 
		{
			return _buffer[num];
		}

		Ogre::Vector3& operator[](const unsigned int num) 
		{
			return _buffer[num];
		}

		// Deletes buffer
		void Clear();
		// Ensures that memory for at least 'count' elements is allocated
		void Reserve(unsigned int count);
	};


	inline void VertexBuffer::AddVertexUnsafe(const Ogre::Vector3& vertex)
		{
#ifdef _DEBUG
		_buffer.push_back(vertex);
#else
		_buffer[_size] = vertex;
#endif
			++_size;
		}
}