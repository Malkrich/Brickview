#include "Pch.h"
#include "TextureUtils.h"

#include <glm/glm.hpp>

namespace Brickview::TextureUtils
{

	uint32_t computeWidthOrHeightFromMipLevel(uint32_t mipLevel, uint32_t baseWidthOrHeight)
	{
		return baseWidthOrHeight * glm::pow(0.5, mipLevel);
	}

}