#include "IndexBuffer.h"
#include <Ogre\OgreVertexIndexData.h>

#include <Memory.h>
#include <Exception.h>

namespace FlagRTS
{
	IndexBuffer::IndexBuffer() :
		_size(0),
		_reserved(0)
	{

	}

	IndexBuffer::~IndexBuffer()
	{
		_buffer.clear();
	}

	void IndexBuffer::FromIndexData(Ogre::IndexData* indexData, const unsigned int offset)
	{
		Clear();
		AddFromIndexData(indexData, offset);
	}

	void IndexBuffer::SetData(unsigned int* buffer, const unsigned int count, const unsigned int offset)
	{
		Clear();
		AddData(buffer, count, offset);
	}

	void IndexBuffer::SetData(const Buffer& buffer, const unsigned int count, const unsigned int offset)
	{
		Clear();
		AddData(buffer, count, offset);
	}

	void IndexBuffer::AddFromIndexData(Ogre::IndexData* indexData, const unsigned int offset)
	{
		// Get vertex count
		const unsigned int addedCount = indexData->indexCount;

		if(_reserved < _size + addedCount)
		{
			Reserve(_size + addedCount);
		}

		Ogre::HardwareIndexBufferSharedPtr hardwareBuffer = indexData->indexBuffer;
		const bool use32bitindexes = (hardwareBuffer->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

		if (use32bitindexes) 
		{
			const unsigned int* curIndex = static_cast<unsigned int*>
				(hardwareBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
			for(unsigned int k = 0; k < addedCount; ++k)
			{
				_buffer.push_back(offset + (*curIndex++));
			}
		}
		else
		{
			const unsigned short* curIndex = static_cast<unsigned short*>
				(hardwareBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
			for(unsigned int k = 0; k < addedCount; ++k)
			{
				_buffer.push_back(offset + static_cast<unsigned int>(*curIndex++));
			}
		}
		_size = _buffer.size();
		hardwareBuffer->unlock();
	}

	void IndexBuffer::AddData(unsigned int* buffer, const unsigned int count, const unsigned int offset)
	{
		if(_reserved < count)
			Reserve(count);

		for(unsigned int i = 0; i < count; ++i)
		{
			_buffer.push_back(buffer[i]+offset);
		}
		_size = _buffer.size();
	}

	void IndexBuffer::AddData(const Buffer& buffer, const unsigned int count, const unsigned int offset)
	{
		if(_reserved < count)
			Reserve(count);

		for(unsigned int i = 0; i < count; ++i)
		{
			_buffer.push_back(buffer[i]+offset);
		}
		_size = _buffer.size();
	}

	void IndexBuffer::Clear()
	{
		_buffer.clear();
		_size = 0;
		_reserved = 0;
	}

	void IndexBuffer::Reserve(unsigned int count)
	{
		if(_reserved >= count)
			return;

		_buffer.reserve(count);
		_reserved = _buffer.capacity();
	}
}