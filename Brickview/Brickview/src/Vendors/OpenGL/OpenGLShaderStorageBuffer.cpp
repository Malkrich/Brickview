#include "Pch.h"
#include "OpenGLShaderStorageBuffer.h"

#include "OpenGLError.h"

#include <glad/glad.h>

namespace Brickview
{

	OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(const ShaderStorageBufferSpecifications& specs, uint32_t size, const void* data)
		: m_specs(specs)
	{
		invalidate(size, data);
	}
	
	OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(const ShaderStorageBufferSpecifications& specs, uint32_t size)
		: m_specs(specs)
	{
		invalidate(size, nullptr);
	}

	OpenGLShaderStorageBuffer::~OpenGLShaderStorageBuffer()
	{
		glDeleteBuffers(1, &m_bufferID);
	}

	void OpenGLShaderStorageBuffer::resize(uint32_t size)
	{
		invalidate(size, nullptr);
	}

	void OpenGLShaderStorageBuffer::setData(const void* data)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_bufferSize, data, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		CHECK_GL_ERROR();
	}

	void OpenGLShaderStorageBuffer::invalidate(uint32_t size, const void* data)
	{
		m_bufferSize = size;
		if (m_bufferID)
		{
			glDeleteBuffers(1, &m_bufferID);
		}

		glGenBuffers(1, &m_bufferID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_bufferSize, data, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_specs.BindingPoint, m_bufferID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		CHECK_GL_ERROR();
	}

}