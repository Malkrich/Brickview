#pragma once

#include "Renderer/Texture2D.h"

namespace Brickview
{

	class OpenGLTexture2D : public Texture2D
	{
	public:
		// TODO: should be handled by a TextureImporter class
		OpenGLTexture2D(const Texture2DSpecifications& specs, const std::filesystem::path& filePath);

		OpenGLTexture2D(const Texture2DSpecifications& specs, uint32_t width, uint32_t height, void* data = nullptr);

		static Ref<OpenGLTexture2D> copy(const Texture2DSpecifications& specs, uint32_t textureSource, uint32_t width, uint32_t height);

		virtual ~OpenGLTexture2D();

		virtual void bind(uint32_t slot = 0) const override;

		virtual const Texture2DSpecifications& getSpecifications() const override { return m_specs; }

		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }

		virtual uint32_t getHandle() const override { return m_textureID; }

	private:
		void invalidate(const void* data);

	private:
		uint32_t m_textureID = 0;

		uint32_t m_width = 0;
		uint32_t m_height = 0;

		Texture2DSpecifications m_specs;
	};

}