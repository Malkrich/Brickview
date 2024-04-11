#include "Pch.h"
#include "Buffer.h"

#include <glad/glad.h>

#include "Core/Core.h"

namespace Utils
{
	using BufferElementType = Brickview::BufferElementType;

	static unsigned int getElementCount(BufferElementType type)
	{
		switch(type)
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

namespace Brickview
{
	/*********************************************************/
	/*************   VERTEX BUFFER   *************************/
	/*********************************************************/

	VertexBuffer::VertexBuffer(unsigned int size, const void* data)
	{
		glGenBuffers(1, &m_bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_bufferID);
	}

	void VertexBuffer::bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
	}

	void VertexBuffer::unbind() const
	{
		glBindBuffer(GL_VERTEX_ARRAY, 0);
	}

	/*********************************************************/
	/*************   INDEX BUFFER   **************************/
	/*********************************************************/

	IndexBuffer::IndexBuffer(unsigned int size, const void* data)
		: m_count(size / sizeof(unsigned int))
	{
		glGenBuffers(1, &m_bufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_bufferID);
	}

	void IndexBuffer::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
	}

	void IndexBuffer::unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	/*********************************************************/
	/*************   VERTEX ARRAY   **************************/
	/*********************************************************/

	VertexArray::VertexArray()
	{    
		glGenVertexArrays(1, &m_bufferID);
	}

	void VertexArray::bind() const
	{
		glBindVertexArray(m_bufferID);
	}

	void VertexArray::unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		glBindVertexArray(m_bufferID);
		vertexBuffer->bind();

		const Layout& layout = vertexBuffer->getLayout();
		unsigned int stride = layout.getStride();
		for(const auto& element : layout)
		{
			unsigned int elementCount = Utils::getElementCount(element.Type);
			GLenum elementType = Utils::getOpenGLElementType(element.Type);
			glVertexAttribPointer(element.Index,
								  elementCount,
								  elementType,
								  element.Normalized ? GL_TRUE : GL_FALSE,
								  stride,
								  (const void*)element.Offset);
			glEnableVertexAttribArray(element.Index);
		}

		m_vertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_bufferID);
		indexBuffer->bind();

		m_indexBuffer = indexBuffer;
	}
}