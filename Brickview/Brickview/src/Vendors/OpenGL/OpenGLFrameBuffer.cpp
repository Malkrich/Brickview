#include "Pch.h"
#include "OpenGLFrameBuffer.h"

#include "OpenGLError.h"
#include "OpenGLTextureUtils.h"

#include <glad/glad.h>

namespace Brickview
{

	namespace Utils
	{

		static void attachColorTexture(uint32_t attachmentID, uint32_t width, uint32_t height, GLint internalFormat, GLenum format, GLenum minFilter, GLenum magFilter, uint32_t attachmentIndex)
		{
			// no deals with multisampling yet
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
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

		static void attachCubemapTexture(uint32_t attachmentID, uint32_t width, uint32_t height, GLint internalFormat, GLenum minFilter, GLenum magFilter, uint32_t attachmentIndex)
		{
			for (uint32_t i = 0; i < 6; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			if (minFilter == GL_LINEAR_MIPMAP_LINEAR)
				glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

			// By default we attach postive X face which is index 0
			// We also attach mipmap level 0 by default
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_CUBE_MAP_POSITIVE_X, attachmentID, 0);

			CHECK_GL_ERROR();
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

		static GLenum attahmentFormatToTextureType(FrameBufferAttachment format)
		{
			switch (format)
			{
				case FrameBufferAttachment::RedInt:
				case FrameBufferAttachment::RGBA8:
					return GL_TEXTURE_2D;
				case FrameBufferAttachment::CubemapFloat16:
				case FrameBufferAttachment::CubemapFloat32:
					return GL_TEXTURE_CUBE_MAP;
			}

			BV_ASSERT(false, "Unknwon frame buffer attachemnt!");
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

		glCreateFramebuffers(1, &m_bufferID);

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
		CHECK_GL_ERROR();
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
			for (size_t i = 0; i < m_colorAttachments.size(); i++)
			{
				if (m_colorAttachmentsSpecs[i].Resizable)
				{
					glDeleteTextures(1, &m_colorAttachments[i]);
					m_colorAttachments[i] = 0;
				}
			}
			if (m_depthAttachmentSpecs.Resizable)
			{
				glDeleteTextures(1, &m_depthAttachment);
				m_depthAttachment = 0;
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID);

		if (!m_colorAttachmentsSpecs.empty())
		{
			m_colorAttachments.resize(m_colorAttachmentsSpecs.size());

			for (size_t i = 0; i < m_colorAttachmentsSpecs.size(); i++)
			{
				const FrameBufferAttachmentSpecs& colorAttachmentSpecs = m_colorAttachmentsSpecs[i];
				uint32_t& attachmentID = m_colorAttachments[i];

				if (!colorAttachmentSpecs.Resizable && attachmentID)
					continue;

				glGenTextures(1, &attachmentID);
				GLenum textureType = Utils::attahmentFormatToTextureType(colorAttachmentSpecs.Format);
				GLenum minFilter = TextureUtils::textureFilterToOpenGLFilter(colorAttachmentSpecs.MinFilter);
				GLenum magFilter = TextureUtils::textureFilterToOpenGLFilter(colorAttachmentSpecs.MagFilter);
				uint32_t mipmapLevels = colorAttachmentSpecs.MipmapLevels;
				glBindTexture(textureType, attachmentID);
				switch (m_colorAttachmentsSpecs[i].Format)
				{
					case FrameBufferAttachment::RedInt:
						Utils::attachColorTexture(attachmentID, m_specs.Width, m_specs.Height, GL_R32I, GL_RED_INTEGER, minFilter, magFilter, i);
						continue;
					case FrameBufferAttachment::RGBA8:
						Utils::attachColorTexture(attachmentID, m_specs.Width, m_specs.Height, GL_RGBA8, GL_RGBA, minFilter, magFilter, i);
						continue;

					case FrameBufferAttachment::CubemapFloat16:
						Utils::attachCubemapTexture(attachmentID, m_specs.Width, m_specs.Height, GL_RGB16F, minFilter, magFilter, i);
						continue;
					case FrameBufferAttachment::CubemapFloat32:
						BV_ASSERT(false, "32 bits cubemap not implemented yet!");
						continue;
				}
			}
		}

		if (m_depthAttachmentSpecs.Format != FrameBufferAttachment::None)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
			glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
			switch (m_depthAttachmentSpecs.Format)
			{
				case FrameBufferAttachment::Depth24Sentil8:
					Utils::attachDepthTexture(m_depthAttachment, m_specs.Width, m_specs.Height, GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8);
			}
		}

		if (!m_colorAttachments.empty())
		{
			// TODO: review verification
			// and memory managment of attachmentIndices
			static std::array<GLenum, 4> attachmentIndices = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3 
			};
			BV_ASSERT(m_colorAttachments.size() <= attachmentIndices.size(), "Brickview does not support more than {} color attachments!", attachmentIndices.size());
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

		BV_ASSERT(specs.Format == FrameBufferAttachment::RedInt, "Wrong frame buffer attachment format!");

		GLenum format = Utils::getGlFormatFromAttachmentFormat(specs.Format);
		glClearTexImage(m_colorAttachments[attachmentIndex], 0, format, GL_INT, &value);
	}

	void OpenGLFrameBuffer::attachCubemapFace(uint32_t attachmentIndex, CubemapFace face, uint32_t mipmapLevel)
	{
		BV_ASSERT(m_colorAttachmentsSpecs[attachmentIndex].Format == FrameBufferAttachment::CubemapFloat16
			|| m_colorAttachmentsSpecs[attachmentIndex].Format == FrameBufferAttachment::CubemapFloat32, "Attachment index {} is not a cubemap attachment!", attachmentIndex);

		uint32_t faceIndex = (uint32_t)face;
		uint32_t attachmentID = m_colorAttachments[attachmentIndex];
		glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, attachmentID, mipmapLevel);

		CHECK_GL_ERROR();
	}

}