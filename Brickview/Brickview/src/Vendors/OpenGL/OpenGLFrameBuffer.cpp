#include "Pch.h"
#include "OpenGLFrameBuffer.h"

#include "OpenGLError.h"
#include "OpenGLTextureUtils.h"

#include <glad/glad.h>

namespace Brickview
{

	namespace Utils
	{

		// ------------------------------------------------------------------
		// Texture 2D attachment
		// ------------------------------------------------------------------
		static void createTexutre2DColorAttachment(uint32_t attachmentID, uint32_t width, uint32_t height, GLint internalFormat, GLint format, GLenum minFilter, GLenum magFilter)
		{
			BV_ASSERT(minFilter != GL_LINEAR_MIPMAP_LINEAR, "Mip map linear filtering not implemented for Texture2D attachments!");

			glBindTexture(GL_TEXTURE_2D, attachmentID);

			// no deals with multisampling yet
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		static void attachColorTexture(uint32_t attachmentID, uint32_t attachmentIndex)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_2D, attachmentID, 0);
		}

		// ------------------------------------------------------------------
		// Cubemap attachment
		// ------------------------------------------------------------------
		static void createTextureCubemapColorAttachment(uint32_t attachmentID, uint32_t width, uint32_t height, GLint internalFormat, GLenum minFilter, GLenum magFilter)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, attachmentID);

			for (uint32_t i = 0; i < 6; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			CHECK_GL_ERROR();
		}

		static void attachCubemapTexture(uint32_t attachmentID, uint32_t attachmentIndex)
		{
			// By default we attach postive X face which is index 0
			// We also attach mipmap level 0 by default
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_CUBE_MAP_POSITIVE_X, attachmentID, 0);

			CHECK_GL_ERROR();
		}

		// ------------------------------------------------------------------
		// Depth attachment
		// ------------------------------------------------------------------
		static void createDepthTexture(uint32_t attachmentID, uint32_t width, uint32_t height, GLint internalFormat, GLenum format)
		{
			glBindTexture(GL_TEXTURE_2D, attachmentID);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_DEPTH_STENCIL, format, nullptr);
		}

		static void attachDepthTexture(uint32_t attachmentID)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, attachmentID, 0);
		}

		// ------------------------------------------------------------------
		// Misc
		// ------------------------------------------------------------------
		static bool isDepthFormat(ColorAttachmentFormat format)
		{
			switch (format)
			{
				case ColorAttachmentFormat::Depth24Sentil8: return true;
			}

			return false;
		}

		static GLenum getGlFormatFromAttachmentFormat(ColorAttachmentFormat format)
		{
			switch (format)
			{
				case ColorAttachmentFormat::RedInt:         return GL_RED_INTEGER;
				case ColorAttachmentFormat::RGBA8:          return GL_RGBA8;
				case ColorAttachmentFormat::Depth24Sentil8: return GL_DEPTH24_STENCIL8;
			}

			BV_ASSERT(false, "Unknown frame buffer attahcment format!");
			return 0;
		}

		static GLenum attachmentFormatToTextureType(ColorAttachmentFormat format)
		{
			switch (format)
			{
				case ColorAttachmentFormat::RedInt:
				case ColorAttachmentFormat::RGBA8:
				case ColorAttachmentFormat::RGFloat16:
					return GL_TEXTURE_2D;
				case ColorAttachmentFormat::CubemapFloat16:
				case ColorAttachmentFormat::CubemapFloat32:
					return GL_TEXTURE_CUBE_MAP;
			}

			BV_ASSERT(false, "Unknwon frame buffer attachemnt!");
			return 0;
		}

	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecifications& specs)
		: m_width(specs.Width)
		, m_height(specs.Height)
	{
		populateAttachments(specs.Attachments);

		glCreateFramebuffers(1, &m_bufferID);

		invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		for (size_t i = 0; i < m_colorAttachments.size(); i++)
		{
			// Delete color attachment if it is not external
			if (!m_colorAttachmentsSpecs[i].ExistingAttachment)
				glDeleteTextures(1, &m_colorAttachments[i]);
		}

		// Delete depth attachment if it is not external
		if (!m_depthAttachmentSpecs.ExistingAttachment)
			glDeleteTextures(1, &m_depthAttachment);

		glDeleteFramebuffers(1, &m_bufferID);
	}

	void OpenGLFrameBuffer::bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID);
		glViewport(0, 0, m_width, m_height);
	}

	void OpenGLFrameBuffer::unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		CHECK_GL_ERROR();
	}

	void OpenGLFrameBuffer::resize(uint32_t width, uint32_t height)
	{
		m_width = width;
		m_height = height;

		if (m_width != 0 && m_height != 0)
			invalidate();
	}

	uint32_t OpenGLFrameBuffer::getColorAttachment(uint32_t index) const
	{
		BV_ASSERT(index < m_colorAttachments.size(), "Color attachement index at {} is out of bound!", index);
		return m_colorAttachments[index];
	}

	void OpenGLFrameBuffer::setColorAttachment(uint32_t index, Ref<Texture> texture)
	{
		BV_ASSERT(index < m_colorAttachmentsSpecs.size(), "Color attachment specs at index {} is out of bound!", index);
		BV_ASSERT(texture, "Invalid texture object!");

		FrameBufferAttachmentSpecs& specs = m_colorAttachmentsSpecs[index];
		specs.ExistingAttachment = texture;
		updateSpecsFromTexture(specs);

		invalidate();
	}

	int32_t OpenGLFrameBuffer::readPixel(uint32_t attachmentIndex, uint32_t mouseX, uint32_t mouseY) const
	{
		BV_ASSERT(attachmentIndex < m_colorAttachmentsSpecs.size(), "Index {} for color attachment is out of bound!", attachmentIndex);

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

		BV_ASSERT(specs.Format == ColorAttachmentFormat::RedInt, "Wrong frame buffer attachment format!");

		GLenum format = Utils::getGlFormatFromAttachmentFormat(specs.Format);
		glClearTexImage(m_colorAttachments[attachmentIndex], 0, format, GL_INT, &value);
	}

	void OpenGLFrameBuffer::attachCubemapFace(uint32_t attachmentIndex, CubemapFace face, uint32_t mipmapLevel)
	{
		BV_ASSERT(m_colorAttachmentsSpecs[attachmentIndex].Format == ColorAttachmentFormat::CubemapFloat16
			|| m_colorAttachmentsSpecs[attachmentIndex].Format == ColorAttachmentFormat::CubemapFloat32, "Attachment index {} is not a cubemap attachment!", attachmentIndex);

		uint32_t faceIndex = (uint32_t)face;
		uint32_t attachmentID = m_colorAttachments[attachmentIndex];
		glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, attachmentID, mipmapLevel);
	}

	void OpenGLFrameBuffer::invalidate()
	{
		if (m_bufferID)
		{
			for (size_t i = 0; i < m_colorAttachments.size(); i++)
			{
				const FrameBufferAttachmentSpecs& attachment = m_colorAttachmentsSpecs[i];
				if (attachment.Resizable && !attachment.ExistingAttachment)
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

				Ref<Texture> existingAttachment = colorAttachmentSpecs.ExistingAttachment;
				GLenum minFilter = OpenGLTextureUtils::textureFilterToOpenGLFilter(colorAttachmentSpecs.MinFilter);
				GLenum magFilter = OpenGLTextureUtils::textureFilterToOpenGLFilter(colorAttachmentSpecs.MagFilter);
				uint32_t mipmapLevels = colorAttachmentSpecs.MipmapLevels;

				if (existingAttachment)
				{
					attachmentID = existingAttachment->getHandle();
				}
				else
				{
					glGenTextures(1, &attachmentID);

					switch (colorAttachmentSpecs.Format)
					{
						case ColorAttachmentFormat::RedInt:
							Utils::createTexutre2DColorAttachment(attachmentID, m_width, m_height, GL_R32I, GL_RED_INTEGER, minFilter, magFilter);
							break;
						case ColorAttachmentFormat::RGFloat16:
							Utils::createTexutre2DColorAttachment(attachmentID, m_width, m_height, GL_RG16F, GL_RG, minFilter, magFilter);
							break;
						case ColorAttachmentFormat::RGBA8:
							Utils::createTexutre2DColorAttachment(attachmentID, m_width, m_height, GL_RGBA8, GL_RGBA, minFilter, magFilter);
							break;

						case ColorAttachmentFormat::CubemapFloat16:
							Utils::createTextureCubemapColorAttachment(attachmentID, m_width, m_height, GL_RGB16F, minFilter, magFilter);
							break;
						case ColorAttachmentFormat::CubemapFloat32:
							BV_ASSERT(false, "32 bits cubemap not implemented yet!");
							break;
					}
				}

				// Attach color texture or cubemap texture
				switch (colorAttachmentSpecs.Format)
				{
					case ColorAttachmentFormat::RedInt:
						Utils::attachColorTexture(attachmentID, i);
						continue;
					case ColorAttachmentFormat::RGFloat16:
						Utils::attachColorTexture(attachmentID, i);
						continue;
					case ColorAttachmentFormat::RGBA8:
						Utils::attachColorTexture(attachmentID, i);
						continue;

					case ColorAttachmentFormat::CubemapFloat16:
						Utils::attachCubemapTexture(attachmentID, i);
						continue;
					case ColorAttachmentFormat::CubemapFloat32:
						BV_ASSERT(false, "32 bits cubemap not implemented yet!");
						continue;
				}
			}
		}

		if (m_depthAttachmentSpecs.Format != ColorAttachmentFormat::None)
		{
			BV_ASSERT(!m_depthAttachmentSpecs.ExistingAttachment, "Depth attachment from extisting image not implemented yet!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
			switch (m_depthAttachmentSpecs.Format)
			{
				case ColorAttachmentFormat::Depth24Sentil8:
					Utils::createDepthTexture(m_depthAttachment, m_width, m_height, GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8);
			}
			Utils::attachDepthTexture(m_depthAttachment);
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

	void OpenGLFrameBuffer::populateAttachments(const std::vector<FrameBufferAttachmentSpecs>& attachments)
	{
		for (const auto& attachment : attachments)
		{
			if (Utils::isDepthFormat(attachment.Format))
			{
				m_depthAttachmentSpecs = attachment;
			}
			else
			{
				FrameBufferAttachmentSpecs& colorAttachment = m_colorAttachmentsSpecs.emplace_back(attachment);

				if (attachment.ExistingAttachment)
					updateSpecsFromTexture(colorAttachment);
			}
		}
	}

	void OpenGLFrameBuffer::updateSpecsFromTexture(FrameBufferAttachmentSpecs& attachmentSpecs)
	{
		Ref<Texture> texture = attachmentSpecs.ExistingAttachment;

		switch (attachmentSpecs.Format)
		{
			case ColorAttachmentFormat::RedInt:
			case ColorAttachmentFormat::RGBA8:
			case ColorAttachmentFormat::RGFloat16:
			{
				const Texture2DSpecifications& specs = texture->as<Texture2D>()->getSpecifications();
				attachmentSpecs.MinFilter = specs.FilterMin;
				attachmentSpecs.MagFilter = specs.FilterMag;
				// TODO: include attachment.MipmapLevels as well
				break;
			}

			case ColorAttachmentFormat::CubemapFloat16:
			case ColorAttachmentFormat::CubemapFloat32:
			{
				const TextureCubemapSpecifications& specs = texture->as<TextureCubemap>()->getSpecifications();
				attachmentSpecs.MinFilter = specs.MinFilter;
				attachmentSpecs.MagFilter = specs.MagFilter;
				attachmentSpecs.MipmapLevels = specs.Levels;
				break;
			}
		}
	}

}