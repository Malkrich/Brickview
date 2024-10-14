#include "Pch.h"
#include "OpenGLTexture2D.h"

#include "OpenGLError.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Brickview
{

	namespace Utils
	{
		uint8_t* loadImage(const std::filesystem::path& filePath, TextureSpecifications& specs)
		{
			std::string strPath = filePath.string();
			int width, height, channels;
			uint8_t* data = (uint8_t*)stbi_load(strPath.c_str(), &width, &height, &channels, 0);

			if (!data)
			{
				BV_LOG_ERROR("Unable to load image data from {}", filePath.string());
				return data;
			}

			specs.Width = width;
			specs.Height = height;
			specs.Channels = channels;

			return data;
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& filePath)
	{
		uint8_t* imageData = Utils::loadImage(filePath, m_specs);

		if (imageData)
			createTexture(imageData);

		stbi_image_free(imageData);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_textureID);
	}

	void OpenGLTexture2D::createTexture(const void* data)
	{
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);

		switch (m_specs.Channels)
		{
		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_specs.Width, m_specs.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			break;
		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_specs.Width, m_specs.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			break;
		default:
			BV_ASSERT(false, "Image format not supported!");
			break;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		CHECK_GL_ERROR();
	}

}