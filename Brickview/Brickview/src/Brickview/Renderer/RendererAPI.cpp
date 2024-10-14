#include "Pch.h"
#include "RendererAPI.h"

#include "Vendors/OpenGL/OpenGLRendererAPI.h"

namespace Brickview
{

	Scope<RendererAPI> RendererAPI::create()
	{
		switch (s_api)
		{
			case API::None: BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;;
			case API::OpenGL: return createScope<OpenGLRendererAPI>();
		}

		BV_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

}