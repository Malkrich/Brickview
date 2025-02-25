#pragma once

#include "Buffer.h"

namespace Brickview
{

	class BufferStreamWriter
	{
	public:
		BufferStreamWriter(Buffer& buffer);

		template<typename T>
		void writePrimitiveType(T data)
		{
			m_buffer.allocate(m_buffer.Size + sizeof(T));

			memcpy(m_buffer.Data + m_bufferPointer, &data, sizeof(T));
			m_bufferPointer += sizeof(T);
		}

		template<typename T>
		void writeSpan(uint32_t count, T data)
		{
			for (uint32_t i = 0; i < count; i++)
				writePrimitiveType<T>(data);
		}

		template<typename T>
		void writeVector(const std::vector<T>& vector)
		{
			uint32_t vectorSize = sizeof(T) * vector.size();
			m_buffer.allocate(m_buffer.Size + vectorSize);

			memcpy(m_buffer.Data + m_bufferPointer, vector.data(), vectorSize);
			m_bufferPointer += vectorSize;
		}

	private:
		Buffer& m_buffer;
		uint32_t m_bufferPointer = 0;
	};

}