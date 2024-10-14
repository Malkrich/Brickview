#include "Pch.h"
#include "Texture2D.h"

#include "RendererAPI.h"
#include "Vendors/OpenGL/OpenGLTexture2D.h"

namespace Brickview
{

	Ref<Texture2D> Texture2D::create(const std::filesystem::path& filePath)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLTexture2D>(filePath);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

}