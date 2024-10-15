#include "Pch.h"
#include "OpenGLUniformBuffer.h"

#include "OpenGLError.h"

#include <glad/glad.h>

namespace Brickview
{

	OpenGLUniformBuffer::OpenGLUniformBuffer(const UniformBufferSpecifications& specs)
		: m_specs(specs)
	{
		glGenBuffers(1, &m_bufferID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID);
		glBufferData(GL_UNIFORM_BUFFER, specs.Layout.getBufferSize(), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, m_specs.BindingPoint, m_bufferID);

		CHECK_GL_ERROR();
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_bufferID);
	}

	void OpenGLUniformBuffer::bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID);
	}

	void OpenGLUniformBuffer::unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::setElement(uint32_t elementIndex, const void* data)
	{
		BV_ASSERT(elementIndex < m_specs.Layout.getElementCount(), "Buffer element index out of bound!");

		uint32_t size = m_specs.Layout.getBufferElement(elementIndex).Size;
		uint32_t offset = m_specs.Layout.getBufferElement(elementIndex).Offset;

		glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		CHECK_GL_ERROR();
	}

}