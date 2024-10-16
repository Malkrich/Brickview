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
			// no deals with multisampling yet
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_2D, attachmentID, 0);
		}

		static void attachDepthTexture(uint32_t attachmentID, uint32_t width, uint32_t height, GLint internalFormat, GLenum format)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_DEPTH_STENCIL, format, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, attachmentID, 0);
		}

		static void createTexture(uint32_t attachmentCount, GLuint* attachmentID)
		{
			glCreateTextures(GL_TEXTURE_2D, attachmentCount, attachmentID);
		}

		static void bindTexture(uint32_t attachmentID)
		{
			glBindTexture(GL_TEXTURE_2D, attachmentID);
		}

		static bool isDepthFormat(FrameBufferAttachment format)
		{
			switch (format)
			{
				case FrameBufferAttachment::Depth24Sentil8: return true;
			}

			return false;
		}

		static GLenum getGlFormatFromAttachmentFormat(FrameBufferAttachment format)
		{
			switch (format)
			{
				case FrameBufferAttachment::RedInt:         return GL_RED_INTEGER;
				case FrameBufferAttachment::RGBA8:          return GL_RGBA8;
				case FrameBufferAttachment::Depth24Sentil8: return GL_DEPTH24_STENCIL8;
			}

			BV_ASSERT(false, "Unknown frame buffer attahcment format!");
			return 0;
		}

	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecifications& specs)
		: m_specs(specs)
	{
		for (const auto& attachment : specs.Attachments)
		{
			if (Utils::isDepthFormat(attachment.Format))
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

		if (m_specs.Width != 0 && m_specs.Height != 0)
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
			Utils::createTexture(m_colorAttachments.size(), m_colorAttachments.data());

			for (size_t i = 0; i < m_colorAttachmentsSpecs.size(); i++)
			{
				uint32_t attachmentID = m_colorAttachments[i];
				Utils::bindTexture(attachmentID);
				switch (m_colorAttachmentsSpecs[i].Format)
				{
					case FrameBufferAttachment::RedInt:
						Utils::attachColorTexture(attachmentID, m_specs.Width, m_specs.Height, GL_R32I, GL_RED_INTEGER, i);
						continue;
					case FrameBufferAttachment::RGBA8:
						Utils::attachColorTexture(attachmentID, m_specs.Width, m_specs.Height, GL_RGBA8, GL_RGBA, i);
						continue;
				}
			}
		}

		Utils::createTexture(1, &m_depthAttachment);
		Utils::bindTexture(m_depthAttachment);

		if (m_depthAttachmentSpecs.Format != FrameBufferAttachment::None)
		{
			switch (m_depthAttachmentSpecs.Format)
			{
				case FrameBufferAttachment::Depth24Sentil8:
					Utils::attachDepthTexture(m_depthAttachment, m_specs.Width, m_specs.Height, GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8);
			}
		}

		if (!m_colorAttachments.empty())
		{
			static constexpr std::array<GLenum, 4> attachmentIndices = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3 
			};
			BV_ASSERT(m_colorAttachments.size() < attachmentIndices.size(), "Brickview does not support more than {} color attachments in one FBO!", attachmentIndices.size());
			glDrawBuffers(m_colorAttachments.size(), attachmentIndices.data());
			CHECK_GL_ERROR();
		}
		else
		{
			glDrawBuffer(GL_NONE);
		}

		BV_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Frame buffer is incomplete !");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	int32_t OpenGLFrameBuffer::readPixel(uint32_t attachmentIndex, uint32_t mouseX, uint32_t mouseY) const
	{
		BV_ASSERT(attachmentIndex < m_colorAttachmentsSpecs.size(), "Index {} of color attachment is out of bound!", attachmentIndex);

		CHECK_GL_ERROR();

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

		int32_t data;
		CHECK_GL_ERROR();

		glReadPixels(mouseX, mouseY, 1, 1, GL_RED_INTEGER, GL_INT, &data);

		CHECK_GL_ERROR();
		return data;
	}

	void OpenGLFrameBuffer::clearAttachment(uint32_t attachmentIndex, int value)
	{
		BV_ASSERT(attachmentIndex < m_colorAttachmentsSpecs.size(), "Index {} of color attachment is out of bound!", attachmentIndex);

		const FrameBufferAttachmentSpecs& specs = m_colorAttachmentsSpecs[attachmentIndex];

		BV_ASSERT(specs.Format == FrameBufferAttachment::RedInt, "Wrong frame bfufer attachment format!");

		GLenum format = Utils::getGlFormatFromAttachmentFormat(specs.Format);
		glClearTexImage(m_colorAttachments[attachmentIndex], 0, format, GL_INT, &value);
	}

}