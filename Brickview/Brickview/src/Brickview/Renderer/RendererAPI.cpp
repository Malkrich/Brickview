#include "Pch.h"
#include "RendererAPI.h"

#include <glad/glad.h>

namespace Brickview
{

	void RendererAPI::init()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void RendererAPI::shutdown()
	{
	}

}