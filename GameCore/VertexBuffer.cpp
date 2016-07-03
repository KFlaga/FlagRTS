#include "VertexBuffer.h"
#include <Ogre\OgreVertexIndexData.h>

#include <Memory.h>
#include <Exception.h>

namespace FlagRTS
{
	VertexBuffer::VertexBuffer() :
		_size(0),
		_reserved(0)
	{

	}

	VertexBuffer::~VertexBuffer()
	{
		_buffer.clear();
	}

	void VertexBuffer::FromVertexData(Ogre::VertexData* vertexData)
	{
		Clear();
		AddFromVertexData(vertexData);
	}

	void VertexBuffer::SetData(Ogre::Vector3* buffer, const unsigned int count)
	{
		Clear();
		AddData(buffer, count);
	}

	void VertexBuffer::SetData(const Buffer& buffer, const unsigned int count)
	{
		Clear();
		AddData(buffer, count);
	}

	void VertexBuffer::AddVertex(const Ogre::Vector3& vertex)
	{
		if(_size >= _reserved)
			Reserve(_size + 1);
		_buffer.push_back(vertex);
		++_size;
	}

	void VertexBuffer::AddFromVertexData(Ogre::VertexData* vertexData)
	{
		// Get vertex count
		const unsigned int addedCount = vertexData->vertexCount;

		if(_reserved < _size + addedCount)
		{
			Reserve(_size + addedCount);
		}

		// Get VertexElement with info on used vertex semantics
		const Ogre::VertexElement* vertexElement = 
			vertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
		// Get actual vertex buffer and its size
		Ogre::HardwareVertexBufferSharedPtr hardwareBuffer = 
			vertexData->vertexBufferBinding->getBuffer(vertexElement->getSource());
		const unsigned int vertexSize = (unsigned int)hardwareBuffer->getVertexSize();
		// Lock buffer with read-only to retrieve vertex data
		unsigned char* buffer = static_cast<unsigned char*>(
			hardwareBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
		// Pointer to one component of vertex vector
		float *curVertexData = NULL;

		for (unsigned int j = 0; j < addedCount; ++j)
		{
			vertexElement->baseVertexPointerToElement(buffer, &curVertexData);
			buffer += vertexSize;

			_buffer.push_back(Ogre::Vector3(
				*curVertexData,
				*(curVertexData+1) ,
				*(curVertexData+2)));

			curVertexData += 3;
		}

		_size = _buffer.size();
		hardwareBuffer->unlock();
	}

	void VertexBuffer::AddData(Ogre::Vector3* buffer, unsigned int count)
	{
		if(_reserved < count)
			Reserve(count);
		
		for(unsigned int i = 0; i < count; ++i)
		{
			_buffer.push_back(buffer[i]);
		}
		_size = _buffer.size();
	}

	void VertexBuffer::AddData(const Buffer& buffer, unsigned int count)
	{
		if(_reserved < count)
			Reserve(count);

		for(unsigned int i = 0; i < count; ++i)
		{
			_buffer.push_back(buffer[i]);
		}
		_size = _buffer.size();
	}

	void VertexBuffer::Clear()
	{
		_buffer.clear();
		_size = 0;
		_reserved = 0;
	}

	void VertexBuffer::Reserve(unsigned int count)
	{
		if(_reserved >= count)
			return;

		_buffer.reserve(count);
		_reserved = _buffer.capacity();
	}
}