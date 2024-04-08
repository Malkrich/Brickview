#include "Pch.h"
#include "Window.h"

#include <glad/glad.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "Core/Log.h"
#include "Core/Event/ApplicationEvent.h"
#include "Core/Event/MouseEvent.h"

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

		// init window
		int version = gladLoadGL();
		if (version == 0)
		{
			BV_LOG_ERROR("Failed to initialize OpenGL context !");
			return;
		}

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

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y)
		{
			WindowSettings wSettings = *(WindowSettings*)glfwGetWindowUserPointer(window);
				
			MouseMoveEvent e((unsigned int)x, (unsigned int)y);
			wSettings.callbackFn(e);
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowSettings wSettings = *(WindowSettings*)glfwGetWindowUserPointer(window);
			
			switch (action)
			{
				case GLFW_PRESS:
				{
					MousePressedEvent e(button);
					wSettings.callbackFn(e);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseReleaseEvent e(button);
					wSettings.callbackFn(e);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double offsetX, double offsetY)
		{
			WindowSettings wSettings = *(WindowSettings*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent e(offsetX, offsetY);
			wSettings.callbackFn(e);
		});
	}
}