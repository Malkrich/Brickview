#include "Pch.h"
#include "Window.h"

#include "Core/Event/ApplicationEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Event/KeyEvent.h"

#include <glad/glad.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
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
		/* Swap front and back buffers */
		glfwSwapBuffers(m_window);

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

		/* Make the window's context current */
		glfwMakeContextCurrent(m_window);

		// init window
		int version = gladLoadGL();
		if (version == 0)
		{
			BV_LOG_ERROR("Failed to initialize OpenGL context !");
			return;
		}

		// set window settings as user pointer
		glfwSetWindowUserPointer(m_window, &m_settings);

		// VSync
		glfwSwapInterval(m_settings.VSync ? 1 : 0);

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