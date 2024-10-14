#include "Pch.h"
#include "OpenGLTexture2D.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Brickview
{

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecifications& specs)
		: m_specs(specs)
	{
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& filePath)
	{
		std::string strPath = filePath.string();
		stbi_uc* imageData = nullptr;
		int width, height, channels;
		imageData = stbi_load(strPath.c_str(), &width, &height, &channels, 0);

		stbi_image_free(imageData);
	}

}