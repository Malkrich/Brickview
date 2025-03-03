#include "Pch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Brickview
{

	OpenGLContext::OpenGLContext(GLFWwindow* window)
		: m_window(window)
	{}

	OpenGLContext::~OpenGLContext()
	{

	}

	void OpenGLContext::init()
	{
		/* Make the window's context current */
		glfwMakeContextCurrent(m_window);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		BV_ASSERT(status, "Failed to initialize Glad!");

		BV_LOG_INFO("Using OpenGL API version {}", (char*)glGetString(GL_VERSION));
		BV_LOG_INFO(" - Vendor: {}", (char*)glGetString(GL_VENDOR));
		BV_LOG_INFO(" - Device: {}", (char*)glGetString(GL_RENDERER));

		BV_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Brickview requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::swapBuffers()
	{
		/* Swap front and back buffers */
		glfwSwapBuffers(m_window);
	}

}