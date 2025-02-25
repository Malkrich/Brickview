#include "Pch.h"
#include "Buffer.h"

namespace Brickview
{

	Buffer::Buffer(uint32_t size)
	{
		allocate(size);
	}

	void Buffer::release()
	{
		delete[] Data;
		Data = nullptr;
		Size = 0;
	}

	void Buffer::allocate(uint32_t size)
	{
		release();

		Data = new uint8_t[size];
		Size = size;
	}

	Buffer Buffer::copy(const Buffer& other)
	{
		BV_ASSERT(other, "Buffer source does not contain data to copy !");

		Buffer newBuffer;
		newBuffer.Size = other.Size;
		memcpy(newBuffer.Data, other.Data, (size_t)newBuffer.Size);
		
		return newBuffer;
	}

}