#pragma once

#include "Brickview/Renderer/TextureSpecifications.h"

#include <glad/glad.h>

namespace Brickview::OpenGLTextureUtils
{

	GLenum textureFormatToOpenGLInternalFormat(TextureFormat format);
	GLenum textureWrapModeToOpenGLWrapMode(TextureWrapMode mode);
	GLenum textureFilterToOpenGLFilter(TextureFilter filter);

}