#pragma once

#include "Renderer/Texture2D.h"

namespace Brickview
{

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecifications& specs);
		OpenGLTexture2D(const std::filesystem::path& filePath);

		virtual const TextureSpecifications& getSpecifications() const override { return m_specs; }

		virtual uint32_t getWidth() const override { return m_specs.Width; }
		virtual uint32_t getHeight() const override { return m_specs.Height; }

		virtual uint32_t getTextureID() const override { return m_textureID; }

	private:
		uint32_t m_textureID = 0;

		TextureSpecifications m_specs;
	};

}