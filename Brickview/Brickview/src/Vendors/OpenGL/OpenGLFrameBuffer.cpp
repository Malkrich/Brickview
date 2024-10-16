#include "Pch.h"
#include "OpenGLFrameBuffer.h"

#include "OpenGLError.h"

#include <glad/glad.h>

namespace Brickview
{

	namespace Utils
	{

		static void attachColorTexture(uint32_t attachmentID, uint32_t width, uint32_t height, GLint internalFormat, GLenum format, uint32_t attachmentIndex)
		{
			glBindTexture(GL_TEXTURE_2D, attachmentID);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); CHECK_GL_ERROR();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); CHECK_GL_ERROR();

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_2D, attachmentID, 0);
		}

		static bool isDepthFormat(FrameBufferAttachment format)
		{
			switch (format)
			{
				case FrameBufferAttachment::Depth24Sentil8: return true;
			}

			return false;
		}

	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecifications& specs)
		: m_specs(specs)
	{
		for (const auto& attachment : specs.Attachments)
		{
			if (!Utils::isDepthFormat(attachment.Format))
				m_depthAttachmentSpecs = attachment;
			else
				m_colorAttachmentsSpecs.push_back(attachment);
		}

		invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_bufferID);
		glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
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

	uint32_t OpenGLFrameBuffer::getColorAttachment(uint32_t index) const
	{
		BV_ASSERT(index < m_colorAttachments.size(), "Color attachement index {} is out of bound!", index);
		return m_colorAttachments[index];
	}

	void OpenGLFrameBuffer::invalidate()
	{
		if (m_bufferID)
		{
			glDeleteFramebuffers(1, &m_bufferID);
			glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
			glDeleteTextures(1, &m_depthAttachment);

			m_colorAttachments.clear();
			m_depthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_bufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID);

		if (!m_colorAttachmentsSpecs.empty())
		{
			m_colorAttachments.resize(m_colorAttachmentsSpecs.size());
			glCreateTextures(GL_TEXTURE_2D, m_colorAttachments.size(), m_colorAttachments.data());

			for (size_t i = 0; i < m_colorAttachmentsSpecs.size(); i++)
			{
				switch (m_colorAttachmentsSpecs[i].Format)
				{
					case FrameBufferAttachment::RedInt:
						Utils::attachColorTexture(m_colorAttachments[i], m_specs.Width, m_specs.Height, GL_R32I, GL_RED_INTEGER, i);
					case FrameBufferAttachment::RGBA8:
						Utils::attachColorTexture(m_colorAttachments[i], m_specs.Width, m_specs.Height, GL_RGBA, GL_RGBA8, i);
				}
			}
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_specs.Width, m_specs.Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment, 0);

		BV_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Frame buffer is incomplete !");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		CHECK_GL_ERROR();
	}

}