#include "Pch.h"
#include "OpenGLTextureCubemap.h"

#include "Renderer/TextureUtils.h"
#include "OpenGLError.h"
#include "OpenGLTextureUtils.h"

#include <glad/glad.h>

namespace Brickview
{

	OpenGLTextureCubemap::OpenGLTextureCubemap(const TextureCubemapSpecifications& specs)
		: m_specs(specs)
	{
		invalidate();
	}

	Ref<OpenGLTextureCubemap> OpenGLTextureCubemap::copy(const TextureCubemapSpecifications& specs, uint32_t textureID)
	{
		Ref<OpenGLTextureCubemap> cubemap = createRef<OpenGLTextureCubemap>(specs);

		for (uint32_t mipLevel = 0; mipLevel < specs.Levels; mipLevel++)
		{
			uint32_t width  = TextureUtils::computeWidthOrHeightFromMipLevel(mipLevel, specs.Width);
			uint32_t height = TextureUtils::computeWidthOrHeightFromMipLevel(mipLevel, specs.Height);

			glCopyImageSubData(textureID, GL_TEXTURE_CUBE_MAP, mipLevel, 0, 0, 0,
				cubemap->m_textureID, GL_TEXTURE_CUBE_MAP, mipLevel, 0, 0, 0, width, height, 6);
		}

		return cubemap;
	}

	OpenGLTextureCubemap::~OpenGLTextureCubemap()
	{
		glDeleteTextures(1, &m_textureID);
	}

	void OpenGLTextureCubemap::bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	}

	void OpenGLTextureCubemap::invalidate()
	{
		if (m_textureID)
		{
			glDeleteTextures(1, &m_textureID);
		}

		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

		GLenum internalFormat = OpenGLTextureUtils::textureFormatToOpenGLTextureInternalFormat(m_specs.Format);
		for (uint32_t faceIndex = 0; faceIndex < 6; faceIndex++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, 0, internalFormat, m_specs.Width, m_specs.Height, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		GLenum minFilter = OpenGLTextureUtils::textureFilterToOpenGLFilter(m_specs.MinFilter);
		GLenum magFilter = OpenGLTextureUtils::textureFilterToOpenGLFilter(m_specs.MagFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);

		if (m_specs.MinFilter == TextureFilter::LinearMipmapLinear)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		CHECK_GL_ERROR();
	}

}