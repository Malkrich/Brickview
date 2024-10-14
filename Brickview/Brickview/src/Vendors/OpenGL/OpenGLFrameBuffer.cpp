#include "Pch.h"
#include "OpenGLFrameBuffer.h"

#include "OpenGLError.h"

#include <glad/glad.h>

namespace Brickview
{

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecifications& specs)
		: m_specs(specs)
	{
		invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_bufferID);
		glDeleteTextures(1, &m_colorAttachment);
		glDeleteTextures(1, &m_depthAttachment);
	}

	void OpenGLFrameBuffer::bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID);
		glViewport(0, 0, m_specs.Width, m_specs.Height);
	}

	void OpenGLFrameBuffer::unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::resize(uint32_t width, uint32_t height)
	{
		m_specs.Width = width;
		m_specs.Height = height;
		invalidate();
	}

	void OpenGLFrameBuffer::invalidate()
	{
		if (m_bufferID)
		{
			glDeleteFramebuffers(1, &m_bufferID);
			glDeleteTextures(1, &m_colorAttachment);
			glDeleteTextures(1, &m_depthAttachment);

			m_colorAttachment = 0;
			m_depthAttachment = 0;
		}
		glCreateFramebuffers(1, &m_bufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_specs.Width, m_specs.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); CHECK_GL_ERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); CHECK_GL_ERROR();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_specs.Width, m_specs.Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment, 0);

		BV_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Frame buffer is incomplete !");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}