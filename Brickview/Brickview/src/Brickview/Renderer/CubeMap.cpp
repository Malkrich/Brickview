#include "Pch.h"
#include "CubeMap.h"

#include "RendererAPI.h"
#include "OpenGL/OpenGLCubeMap.h"

namespace Brickview
{

	Ref<CubeMap> CubeMap::create()
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLCubeMap>();
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

}