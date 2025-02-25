#include "Pch.h"
#include "OpenGLUniformBuffer.h"

#include "OpenGLError.h"

#include <glad/glad.h>

namespace Brickview
{

	OpenGLUniformBuffer::OpenGLUniformBuffer(const UniformBufferSpecifications& specs, uint32_t size)
		: m_specs(specs)
	{
		invalidate(size, nullptr);
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(const UniformBufferSpecifications& specs, uint32_t size, const void* data)
		: m_specs(specs)
	{
		invalidate(size, data);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_bufferID);
	}

	void OpenGLUniformBuffer::setData(const void* data)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID);
		glBufferData(GL_UNIFORM_BUFFER, m_bufferSize, data, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		CHECK_GL_ERROR();
	}
	
	void OpenGLUniformBuffer::resize(uint32_t newSize)
	{
		invalidate(newSize, nullptr);
	}

	void OpenGLUniformBuffer::invalidate(uint32_t size, const void* data)
	{
		m_bufferSize = size;
		if (m_bufferID)
		{
			glDeleteBuffers(1, &m_bufferID);
		}

		glGenBuffers(1, &m_bufferID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID);
		glBufferData(GL_UNIFORM_BUFFER, m_bufferSize, data, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, m_specs.BindingPoint, m_bufferID);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		CHECK_GL_ERROR();
	}

}