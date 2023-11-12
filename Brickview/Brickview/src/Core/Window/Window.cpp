#include "Window.h"

#include "Core\Log.h"

#include "GLFW\glfw3.h"

namespace Brickview
{
	Window::Window(const WindowSettings& windowSettings)
	{
		initialize(windowSettings);
	}

	Window::~Window()
	{
		glfwTerminate();
	}

	void Window::onUpdate()
	{
		/* Swap front and back buffers */
		glfwSwapBuffers(m_window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	void Window::initialize(const WindowSettings& windowSettings)
	{
		m_settings.name = windowSettings.name;
		m_settings.width = windowSettings.width;
		m_settings.height = windowSettings.height;

		/* Initialize the library */
		if (!glfwInit())
		{
			BV_LOG_ERROR("glfwInit() failed !");
			return;
		}

		/* Create a windowed mode window and its OpenGL context */
		m_window = glfwCreateWindow(m_settings.width, m_settings.height,
			m_settings.name.c_str(),
			NULL, NULL);

		if (!m_window)
		{
			BV_LOG_ERROR("window creation failed !");
			glfwTerminate();
			return;
		}

		/* Make the window's context current */
		glfwMakeContextCurrent(m_window);

		// set window settings as user pointer
		glfwSetWindowUserPointer(m_window, &m_settings);

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
		{
			WindowSettings wSettings = *(WindowSettings*)glfwGetWindowUserPointer(window);
			WindowCloseEvent e;
			wSettings.callbackFn(e);
		});

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
		{
			WindowSettings wSettings = *(WindowSettings*)glfwGetWindowUserPointer(window);

			WindowResizeEvent e(width, height);
			wSettings.callbackFn(e);

			wSettings.width = width;
			wSettings.height = height;
		});
	}
}