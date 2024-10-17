#include "Pch.h"
#include "OpenGLBuffer.h"

#include "OpenGLError.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Brickview
{
	namespace Utils
	{
		using BufferElementType = Brickview::BufferElementType;

		static uint32_t getElementCount(BufferElementType type)
		{
			switch (type)
			{
			case BufferElementType::Bool:	return 1;
			case BufferElementType::Int:	return 1;
			case BufferElementType::Int2:	return 2;
			case BufferElementType::Int3:	return 3;
			case BufferElementType::Int4:	return 4;
			case BufferElementType::Float:	return 1;
			case BufferElementType::Float2: return 2;
			case BufferElementType::Float3: return 3;
			case BufferElementType::Float4: return 4;
			case BufferElementType::Mat2:	return 2 * 2;
			case BufferElementType::Mat3:	return 3 * 3;
			case BufferElementType::Mat4:	return 4 * 4;
			}

			BV_ASSERT(false, "Buffer element type unknown !");
			return 0;
		}

		static void enableFloatAttribute(const BufferElement& element, uint32_t stride)
		{
			uint32_t elementCount = Utils::getElementCount(element.Type);
			glVertexAttribPointer(element.Index, elementCount, GL_FLOAT, element.Normalized ? GL_TRUE : GL_FALSE, stride, (const void*)element.Offset);
			glEnableVertexAttribArray(element.Index);

			// For instance rendering
			if (element.AttributeDivisor > 0)
			{
				glVertexAttribDivisor(element.Index, element.AttributeDivisor);
			}
		}

		static void enableIntegerAttribute(const BufferElement& element, uint32_t stride)
		{
			uint32_t elementCount = Utils::getElementCount(element.Type);
			glVertexAttribIPointer(element.Index, elementCount, GL_INT, stride, (const void*)element.Offset);
			glEnableVertexAttribArray(element.Index);

			// For instance rendering
			if (element.AttributeDivisor > 0)
			{
				glVertexAttribDivisor(element.Index, element.AttributeDivisor);
			}
		}

		static void enableMat4Attribute(const BufferElement& element, uint32_t stride)
		{
			glVertexAttribPointer(element.Index + 0, 4, GL_FLOAT, element.Normalized ? GL_TRUE : GL_FALSE, stride, (void*)(element.Offset + 0 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(element.Index + 0);

			glVertexAttribPointer(element.Index + 1, 4, GL_FLOAT, element.Normalized ? GL_TRUE : GL_FALSE, stride, (void*)(element.Offset + 1 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(element.Index + 1);

			glVertexAttribPointer(element.Index + 2, 4, GL_FLOAT, element.Normalized ? GL_TRUE : GL_FALSE, stride, (void*)(element.Offset + 2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(element.Index + 2);

			glVertexAttribPointer(element.Index + 3, 4, GL_FLOAT, element.Normalized ? GL_TRUE : GL_FALSE, stride, (void*)(element.Offset + 3 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(element.Index + 3);

			// For instance rendering
			if (element.AttributeDivisor > 0)
			{
				glVertexAttribDivisor(element.Index + 0, element.AttributeDivisor);
				glVertexAttribDivisor(element.Index + 1, element.AttributeDivisor);
				glVertexAttribDivisor(element.Index + 2, element.AttributeDivisor);
				glVertexAttribDivisor(element.Index + 3, element.AttributeDivisor);
			}
		}
	}

	/*********************************************************/
	/*************   VERTEX BUFFER   *************************/
	/*********************************************************/

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, const void* data)
	{
		glGenBuffers(1, &m_bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

		CHECK_GL_ERROR();
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		glGenBuffers(1, &m_bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

		CHECK_GL_ERROR();
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_bufferID);
	}

	void OpenGLVertexBuffer::setData(uint32_t size, void* data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		// We first need to invalidate the buffer data and the copy the new buffer data
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	}

	void OpenGLVertexBuffer::bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
	}

	void OpenGLVertexBuffer::unbind() const
	{
		glBindBuffer(GL_VERTEX_ARRAY, 0);
	}

	/*********************************************************/
	/*************   INDEX BUFFER   **************************/
	/*********************************************************/

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t size, const void* data)
		: m_count(size / sizeof(uint32_t))
	{
		glGenBuffers(1, &m_bufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

		CHECK_GL_ERROR();
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t size)
		: m_count(size / sizeof(uint32_t))
	{
		glGenBuffers(1, &m_bufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

		CHECK_GL_ERROR();
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_bufferID);
	}

	void OpenGLIndexBuffer::setData(uint32_t size, void* data)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
		// We first need to invalidate the buffer data and the copy the new buffer data
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
		m_count = size / sizeof(unsigned int);
	}

	void OpenGLIndexBuffer::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
	}

	void OpenGLIndexBuffer::unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	/*********************************************************/
	/*************   VERTEX ARRAY   **************************/
	/*********************************************************/

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_bufferID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_bufferID);
	}

	void OpenGLVertexArray::bind() const
	{
		glBindVertexArray(m_bufferID);
	}

	void OpenGLVertexArray::unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		bind();
		vertexBuffer->bind();

		const Layout& layout = vertexBuffer->getLayout();
		uint32_t stride = layout.getStride();

		for(const BufferElement& element : layout)
		{
			switch (element.Type)
			{
				case BufferElementType::Mat4:
					Utils::enableMat4Attribute(element, stride);
					break;
				case BufferElementType::Int:
				case BufferElementType::Int2:
				case BufferElementType::Int3:
				case BufferElementType::Int4:
					Utils::enableIntegerAttribute(element, stride);
					break;
				case BufferElementType::Float:
				case BufferElementType::Float2:
				case BufferElementType::Float3:
				case BufferElementType::Float4:
					Utils::enableFloatAttribute(element, stride);
					break;
				default:
					BV_ASSERT(false, "Buffer layout element not implemented yet!");
			}
		}

		m_vertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		bind();
		indexBuffer->bind();

		m_indexBuffer = indexBuffer;
	}
}