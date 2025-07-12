#pragma once

#include "Brickview/Renderer/TextureSpecifications.h"

#include <glad/glad.h>

namespace Brickview::OpenGLTextureUtils
{

	GLenum textureFormatToOpenGLTextureInternalFormat(TextureFormat format);

	GLenum textureFormatToOpenGLTextureFormat(TextureFormat format);

	GLenum textureFormatToOpenGLTextureType(TextureFormat format);

	GLenum textureWrapModeToOpenGLWrapMode(TextureWrapMode mode);

	GLenum textureFilterToOpenGLFilter(TextureFilter filter);

}