#pragma once

#include <Array.h>

namespace Ogre
{
	class IndexData;
}

namespace FlagRTS
{
	// Simple buffer of Vector3, representing positions of vertices of mesh
	class IndexBuffer
	{
	private:
		typedef Array<unsigned int> Buffer;
		Buffer _buffer;
		unsigned int _size;
		unsigned int _reserved;

	public:
		IndexBuffer();
		~IndexBuffer();

		// Clears buffer and sets buffer based on given IndexData
		// Offset is added to each index, used if vertex buffer contains multiple meshes
		void FromIndexData(Ogre::IndexData* indexData, const unsigned int offset);

		// Clears buffer and sets buffer based on given data
		// ( stores copy of data )
		// Offset is added to each index, used if vertex buffer contains multiple meshes
		void SetData(unsigned int* buffer, const unsigned int count, const unsigned int offset);

		// Clears buffer and sets buffer based on given data
		// ( stores copy of data )
		// Offset is added to each index, used if vertex buffer contains multiple meshes
		void SetData(const Buffer& buffer, const unsigned int count, const unsigned int offset);
		
		// Adds to buffer indices from given IndexData, allocating new
		// buffer if there is not enough reserved memory
		// Offset is added to each index, used if vertex buffer contains multiple meshes
		void AddFromIndexData(Ogre::IndexData* indexData, const unsigned int offset);

		// Adds to buffer indices from given data, allocating new
		// buffer if there is not enough reserved memory
		// ( stores copy of data )
		// Offset is added to each index, used if vertex buffer contains multiple meshes
		void AddData(unsigned int* buffer, const unsigned int count, const unsigned int offset);

		// Adds to buffer indices from given data, allocating new
		// buffer if there is not enough reserved memory
		// ( stores copy of data )
		// Offset is added to each index, used if vertex buffer contains multiple meshes
		void AddData(const Buffer& buffer, const unsigned int count, const unsigned int offset);

		// Adds single index to buffer
		void AddIndex(unsigned int index);

		unsigned int GetIndexCount() const { return _size; }
		const Buffer& GetData() const { return _buffer; }

		unsigned int GetIndex(unsigned int num) const 
		{
			return _buffer[num];
		}

		unsigned int& GetIndex(unsigned int num) 
		{
			return _buffer[num];
		}

		// Deletes buffer
		void Clear();
		// Ensures that memory for at least 'count' elements is allocated
		void Reserve(unsigned int count);
	};
}