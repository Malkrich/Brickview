#pragma once

#include "Renderer/TextureCubemap.h"

namespace Brickview
{

	class OpenGLTextureCubemap : public TextureCubemap
	{
	public:
		OpenGLTextureCubemap(const TextureCubemapSpecifications& specs);
		virtual ~OpenGLTextureCubemap();

		static Ref<OpenGLTextureCubemap> copy(const TextureCubemapSpecifications& specs, uint32_t textureID);

		virtual uint32_t getHandle() const override { return m_textureID; }

		virtual uint32_t getWidth() const override { return m_specs.Width; }
		virtual uint32_t getHeight() const override { return m_specs.Height; }

		virtual void bind(uint32_t slot = 0) const override;

		virtual const TextureCubemapSpecifications& getSpecifications() const override { return m_specs; }

	private:
		void invalidate();

	private:
		uint32_t m_textureID = 0;

		TextureCubemapSpecifications m_specs;
	};

}