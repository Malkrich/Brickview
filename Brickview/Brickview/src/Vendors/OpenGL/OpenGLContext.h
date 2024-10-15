#pragma once

#include "Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Brickview
{

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* window);

		virtual ~OpenGLContext();

		virtual void init() override;
		virtual void swapBuffers() override;

	private:
		GLFWwindow* m_window;
	};

}