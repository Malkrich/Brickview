#include "Pch.h"
#include "TextureCubemap.h"

#include "RendererAPI.h"
#include "Vendors/OpenGL/OpenGLTextureCubemap.h"

namespace Brickview
{

	Ref<TextureCubemap> TextureCubemap::create(const TextureCubemapSpecifications& specs)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLTextureCubemap>(specs);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

	Ref<TextureCubemap> TextureCubemap::copy(const TextureCubemapSpecifications& specs, uint32_t textureID)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return OpenGLTextureCubemap::copy(specs, textureID);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

}