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
			uint32_t typeSize = sizeof(T);
			m_buffer.resize(m_buffer.Size + typeSize);

			void* destPtr = m_buffer.Data + m_bufferPointer;
			memcpy(destPtr, &data, typeSize);

			m_bufferPointer += typeSize;
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
			m_buffer.resize(m_buffer.Size + vectorSize);

			memcpy(m_buffer.Data + m_bufferPointer, vector.data(), vectorSize);
			m_bufferPointer += vectorSize;
		}

	private:
		Buffer& m_buffer;
		uint32_t m_bufferPointer = 0;
	};

}