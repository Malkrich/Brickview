#pragma once

#include "Renderer/Cubemap.h"

namespace Brickview
{

	class OpenGLCubemap : public Cubemap
	{
	public:
		OpenGLCubemap(const CubemapSpecifications& specs);
		virtual ~OpenGLCubemap();

		static Ref<OpenGLCubemap> copy(const CubemapSpecifications& specs, uint32_t textureID);

		virtual void bind(uint32_t slot = 0) const override;

	private:
		void invalidate();

	private:
		uint32_t m_textureID = 0;

		CubemapSpecifications m_specs;
	};

}