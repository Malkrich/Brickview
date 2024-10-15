#include "Pch.h"
#include "Window.h"

#include "Core/Event/ApplicationEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Event/KeyEvent.h"

#include <GLFW/glfw3.h>

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
		m_context->swapBuffers();

		/* Poll for and process events */
		glfwPollEvents();
	}

	void Window::initialize(const WindowSettings& windowSettings)
	{
		m_settings = windowSettings;
		
		/* Initialize the library */
		if (!glfwInit())
		{
			BV_LOG_ERROR("glfwInit() failed !");
			return;
		}

		/* Create a windowed mode window and its OpenGL context */
		m_window = glfwCreateWindow(m_settings.Width, m_settings.Height,
			m_settings.Name.c_str(),
			NULL, NULL);

		if (!m_window)
		{
			BV_LOG_ERROR("window creation failed !");
			glfwTerminate();
			return;
		}

		m_context = GraphicsContext::create(m_window);
		m_context->init();

		// set window settings as user pointer
		glfwSetWindowUserPointer(m_window, &m_settings);

		// VSync
		setVSync(windowSettings.VSync);

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
		{
			WindowSettings wSettings = *(WindowSettings*)glfwGetWindowUserPointer(window);
			WindowCloseEvent e;
			wSettings.CallbackFn(e);
		});

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
		{
			WindowSettings wSettings = *(WindowSettings*)glfwGetWindowUserPointer(window);

			WindowResizeEvent e(width, height);
			wSettings.CallbackFn(e);

			wSettings.Width = width;
			wSettings.Height = height;
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowSettings wSettings = *(WindowSettings*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent e(key);
					wSettings.CallbackFn(e);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent e(key);
					wSettings.CallbackFn(e);
					break;
				}
			}
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y)
		{
			WindowSettings wSettings = *(WindowSettings*)glfwGetWindowUserPointer(window);

			MouseMovedEvent e((unsigned int)x, (unsigned int)y);
			wSettings.CallbackFn(e);
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowSettings wSettings = *(WindowSettings*)glfwGetWindowUserPointer(window);
			
			switch (action)
			{
				case GLFW_PRESS:
				{
					MousePressedEvent e(button);
					wSettings.CallbackFn(e);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseReleasedEvent e(button);
					wSettings.CallbackFn(e);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double offsetX, double offsetY)
		{
			WindowSettings wSettings = *(WindowSettings*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent e(offsetX, offsetY);
			wSettings.CallbackFn(e);
		});
	}

	void Window::setVSync(bool enable)
	{
		glfwSwapInterval(enable ? 1 : 0);
		m_settings.VSync = enable;
	}
}