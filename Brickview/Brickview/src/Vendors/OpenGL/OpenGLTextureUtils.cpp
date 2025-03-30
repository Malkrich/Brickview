#include "Pch.h"
#include "OpenGLTextureUtils.h"

#include <glm/glm.hpp>

namespace Brickview::OpenGLTextureUtils
{

	GLenum textureFormatToOpenGLInternalFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGBFloat16: return GL_RGB16F;
		case TextureFormat::RGBFLoat32: return GL_RGB32F;
		}

		BV_ASSERT(false, "Unknown texture format!");
		return 0;
	}

	GLenum textureWrapModeToOpenGLWrapMode(TextureWrapMode mode)
	{
		switch (mode)
		{
			case TextureWrapMode::Repeat:       return GL_REPEAT;
			case TextureWrapMode::ClampToEdge:  return GL_CLAMP;
		}

		BV_ASSERT(false, "Unknown texture wrap mode!");
		return GL_REPEAT;
	}

	GLenum textureFilterToOpenGLFilter(TextureFilter filter)
	{
		switch (filter)
		{
			case TextureFilter::Linear:             return GL_LINEAR;
			case TextureFilter::Nearest:            return GL_NEAREST;
			case TextureFilter::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
		}

		BV_ASSERT(false, "Unknown texture filter!");
		return GL_LINEAR;
	}

}