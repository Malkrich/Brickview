#include "Pch.h"
#include "GraphicsContext.h"

#include "Renderer/RendererAPI.h"
#include "Vendors/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>

namespace Brickview
{

	Scope<GraphicsContext> GraphicsContext::create(void* window)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

}