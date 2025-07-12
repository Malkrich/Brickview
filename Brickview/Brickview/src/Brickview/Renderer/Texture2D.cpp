#include "Pch.h"
#include "Texture2D.h"

#include "RendererAPI.h"
#include "Vendors/OpenGL/OpenGLTexture2D.h"

namespace Brickview
{

	Ref<Texture2D> Texture2D::create(const Texture2DSpecifications specs, const std::filesystem::path& filePath)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLTexture2D>(specs, filePath);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::create(const Texture2DSpecifications specs, uint32_t width, uint32_t height)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLTexture2D>(specs, width, height);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::create(const Texture2DSpecifications specs, uint32_t dimensions)
	{
		return create(specs, dimensions, dimensions);
	}

	Ref<Texture2D> Texture2D::copy(const Texture2DSpecifications& specs, uint32_t sourceTexture, uint32_t width, uint32_t height)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return OpenGLTexture2D::copy(specs, sourceTexture, width, height);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::copy(const Texture2DSpecifications& specs, uint32_t sourceTexture, uint32_t dimensions)
	{
		return copy(specs, sourceTexture, dimensions, dimensions);
	}

}