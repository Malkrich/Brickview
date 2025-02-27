#include "Pch.h"
#include "Cubemap.h"

#include "RendererAPI.h"
#include "OpenGL/OpenGLCubemap.h"

namespace Brickview
{

	Ref<Cubemap> create(const CubemapSpecifications& specs)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLCubemap>(specs);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

	Ref<Cubemap> Cubemap::copy(const CubemapSpecifications& specs, uint32_t textureID)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return OpenGLCubemap::copy(specs, textureID);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

}