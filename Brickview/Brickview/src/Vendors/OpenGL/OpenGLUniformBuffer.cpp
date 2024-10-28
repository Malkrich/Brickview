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
		glBufferData(GL_UNIFORM_BUFFER, m_specs.Layout.getBufferSize(), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, m_specs.BindingPoint, m_bufferID);

		CHECK_GL_ERROR();
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_bufferID);
	}

	void OpenGLUniformBuffer::setElements(const void* data)
	{
		uint32_t size = m_specs.Layout.getBufferSize();

		glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		CHECK_GL_ERROR();
	}

	void OpenGLUniformBuffer::setElement(uint32_t elementIndex, const void* data)
	{
		const UniformBufferElement& bufferElement = m_specs.Layout.getBufferElement(elementIndex);
		uint32_t size   = bufferElement.Size;
		uint32_t offset = bufferElement.Offset;

		glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		CHECK_GL_ERROR();
	}

}