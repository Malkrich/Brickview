#include "Pch.h"
#include "OpenGLCubemap.h"

#include "OpenGLError.h"

#include <glad/glad.h>

namespace Brickview
{

	namespace Utils
	{

		static GLenum cubemapFormatToOpenGLInternalFormat(CubemapFormat format)
		{
			switch (format)
			{
				case CubemapFormat::Float16: return GL_RGB16F;
				case CubemapFormat::Float32: return GL_RGB32F;
			}

			BV_ASSERT(false, "Unknown cubemap format!");
			return 0;
		}

	}

	OpenGLCubemap::OpenGLCubemap(const CubemapSpecifications& specs)
		: m_specs(specs)
	{
		invalidate();
	}

	Ref<OpenGLCubemap> OpenGLCubemap::copy(const CubemapSpecifications& specs, uint32_t textureID)
	{
		Ref<OpenGLCubemap> cubemap = createRef<OpenGLCubemap>(specs);

		glCopyImageSubData(textureID, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
			cubemap->m_textureID, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, specs.Width, specs.Height, 6);
		return cubemap;
	}

	OpenGLCubemap::~OpenGLCubemap()
	{
		glDeleteTextures(1, &m_textureID);
	}

	void OpenGLCubemap::bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	}

	void OpenGLCubemap::invalidate()
	{
		if (m_textureID)
		{
			glDeleteTextures(1, &m_textureID);
		}

		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

		GLenum internalFormat = Utils::cubemapFormatToOpenGLInternalFormat(m_specs.Format);
		for (uint32_t faceIndex = 0; faceIndex < 6; faceIndex++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, 0, internalFormat, m_specs.Width, m_specs.Height, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		CHECK_GL_ERROR();
	}

}