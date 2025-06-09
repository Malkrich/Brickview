#include "Pch.h"
#include "OpenGLTexture2D.h"

#include "OpenGLError.h"
#include "OpenGLTextureUtils.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Brickview
{

	OpenGLTexture2D::OpenGLTexture2D(const Texture2DSpecifications& specs, const std::filesystem::path& filePath)
		: m_specs(specs)
	{
		void* imageData = nullptr;
		if (std::filesystem::exists(filePath))
		{
			std::string strPath = filePath.string();
			int width, height;
			switch (m_specs.Format)
			{
				case TextureFormat::RGB:
				case TextureFormat::RGBA:
					imageData = (void*)stbi_load(strPath.c_str(), &width, &height, nullptr, 0);
					break;
				case TextureFormat::RGBFloat16:
					imageData = (void*)stbi_loadf(strPath.c_str(), &width, &height, nullptr, 0);
					break;
			}
			m_width = width;
			m_height = height;
			BV_ASSERT(imageData, "Unable to load image data from {}", filePath.string());
		}

		if (imageData)
			invalidate(imageData);

		stbi_image_free(imageData);
	}

	OpenGLTexture2D::OpenGLTexture2D(const Texture2DSpecifications& specs, uint32_t width, uint32_t height)
		: m_specs(specs)
		, m_width(width)
		, m_height(height)
	{
		invalidate(nullptr);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_textureID);
	}

	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
	}

	void OpenGLTexture2D::invalidate(const void* data)
	{
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);

		switch(m_specs.Format)
		{
			case TextureFormat::RGFloat16:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, m_width, m_height, 0, GL_RG, GL_FLOAT, data);
				break;
			case TextureFormat::RGBInt32:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				break;
			case TextureFormat::RGBAInt32:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				break;
			case TextureFormat::RGBFloat16:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, data);
				break;
			default:
				BV_ASSERT(false, "Texture format not implemented!");
				break;
		}

		GLenum wrapSParam = OpenGLTextureUtils::textureWrapModeToOpenGLWrapMode(m_specs.WrappingModeU);
		GLenum wrapTParam = OpenGLTextureUtils::textureWrapModeToOpenGLWrapMode(m_specs.WrappingModeV);
		GLenum filterMinParam = OpenGLTextureUtils::textureFilterToOpenGLFilter(m_specs.FilterMin);
		GLenum filterMagParam = OpenGLTextureUtils::textureFilterToOpenGLFilter(m_specs.FilterMag);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapSParam);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapTParam);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMinParam);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMagParam);

		CHECK_GL_ERROR();
	}

}