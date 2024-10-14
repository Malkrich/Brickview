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

		static GLenum getOpenGLElementType(BufferElementType type)
		{
			switch (type)
			{
			case BufferElementType::Bool:	return GL_BOOL;
			case BufferElementType::Int:	return GL_INT;
			case BufferElementType::Int2:	return GL_INT;
			case BufferElementType::Int3:	return GL_INT;
			case BufferElementType::Int4:	return GL_INT;
			case BufferElementType::Float:	return GL_FLOAT;
			case BufferElementType::Float2: return GL_FLOAT;
			case BufferElementType::Float3: return GL_FLOAT;
			case BufferElementType::Float4: return GL_FLOAT;
			case BufferElementType::Mat2:	return GL_FLOAT;
			case BufferElementType::Mat3:	return GL_FLOAT;
			case BufferElementType::Mat4:	return GL_FLOAT;
			}

			BV_ASSERT(false, "Buffer element type unknown !");
			return 0;
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
		unsigned int stride = layout.getStride();
		for(const auto& element : layout)
		{
			unsigned int elementCount = Utils::getElementCount(element.Type);
			GLenum elementType = Utils::getOpenGLElementType(element.Type);
			switch (element.Type)
			{
			case BufferElementType::Mat4:
				// /!\: This implementation is for instance rendering only
				glVertexAttribPointer(element.Index + 0, 4, elementType, element.Normalized ? GL_TRUE : GL_FALSE, 4 * sizeof(glm::vec4), (void*)(0 * sizeof(glm::vec4)));
				glEnableVertexAttribArray(element.Index + 0);
				glVertexAttribPointer(element.Index + 1, 4, elementType, element.Normalized ? GL_TRUE : GL_FALSE, 4 * sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4)));
				glEnableVertexAttribArray(element.Index + 1);
				glVertexAttribPointer(element.Index + 2, 4, elementType, element.Normalized ? GL_TRUE : GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
				glEnableVertexAttribArray(element.Index + 2);
				glVertexAttribPointer(element.Index + 3, 4, elementType, element.Normalized ? GL_TRUE : GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));
				glEnableVertexAttribArray(element.Index + 3);

				// Move this because it depends whether it is instance rendering or not
				glVertexAttribDivisor(element.Index + 0, 1);
				glVertexAttribDivisor(element.Index + 1, 1);
				glVertexAttribDivisor(element.Index + 2, 1);
				glVertexAttribDivisor(element.Index + 3, 1);
				break;
			default:
				glVertexAttribPointer(element.Index,
									  elementCount,
									  elementType,
									  element.Normalized ? GL_TRUE : GL_FALSE,
									  stride,
									  (const void*)element.Offset);
				glEnableVertexAttribArray(element.Index);
				break;
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