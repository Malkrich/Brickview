#include "Pch.h"
#include "Texture2D.h"

#include "stb_image.h"

namespace Brickview
{

	Texture2D::Texture2D(const std::filesystem::path& filePath)
	{
		std::string strPath = filePath.string();
		stbi_uc* imageData = nullptr;
		int width, height, channels;
		imageData = stbi_load(strPath.c_str(), &width, &height, &channels, 0);

		stbi_image_free(imageData);
	}

}