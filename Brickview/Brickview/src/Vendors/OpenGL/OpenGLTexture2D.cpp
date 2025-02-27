#include "Pch.h"
#include "OpenGLTexture2D.h"

#include "OpenGLError.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Brickview
{

	namespace Utils
	{

		static GLenum Texture2DWrapModeToOpenGLWrapMode(Texture2DWrappingMode mode)
		{
			switch (mode)
			{
				case Texture2DWrappingMode::Repeat: return GL_REPEAT;
				case Texture2DWrappingMode::Clamp:  return GL_CLAMP;
			}

			BV_ASSERT(false, "Unknown texture 2D wrap mode!");
			return GL_REPEAT;
		}

		static GLenum Texture2DFilterToOpenGLFilter(Texture2DFilter filter)
		{
			switch (filter)
			{
				case Texture2DFilter::Linear:  return GL_LINEAR;
				case Texture2DFilter::Nearest: return GL_NEAREST;
			}

			BV_ASSERT(false, "Unknown texture 2D filter!");
			return GL_LINEAR;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const Texture2DSpecifications& specs, const std::filesystem::path& filePath)
		: m_specs(specs)
	{
		void* imageData = nullptr;
		if (std::filesystem::exists(filePath))
		{
			std::string strPath = filePath.string();
			int width, height, channels;
			switch (m_specs.Format)
			{
				case Texture2DFormat::RGB:
				case Texture2DFormat::RGBA:
					imageData = (void*)stbi_load(strPath.c_str(), &width, &height, &channels, 0);
					break;
				case Texture2DFormat::FloatRGB:
					imageData = (void*)stbi_loadf(strPath.c_str(), &width, &height, &channels, 0);
					break;
			}
			m_width = width;
			m_height = height;
			m_channels = channels;
			BV_ASSERT(imageData, "Unable to load image data from {}", filePath.string());
		}

		if (imageData)
			invalidate(imageData);

		stbi_image_free(imageData);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_textureID);
	}

	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		//glBindTextureUnit(slot, m_textureID);
	}

	void OpenGLTexture2D::invalidate(const void* data)
	{
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);

		switch(m_specs.Format)
		{
			case Texture2DFormat::RGB:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				break;
			case Texture2DFormat::RGBA:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				break;
			case Texture2DFormat::FloatRGB:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, data);
				break;
		}

		GLenum wrapSParam = Utils::Texture2DWrapModeToOpenGLWrapMode(m_specs.WrappingModeU);
		GLenum wrapTParam = Utils::Texture2DWrapModeToOpenGLWrapMode(m_specs.WrappingModeV);
		GLenum filterMinParam = Utils::Texture2DFilterToOpenGLFilter(m_specs.FilterMin);
		GLenum filterMagParam = Utils::Texture2DFilterToOpenGLFilter(m_specs.FilterMag);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapSParam);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapTParam);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMinParam);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMagParam);

		CHECK_GL_ERROR();
	}

}