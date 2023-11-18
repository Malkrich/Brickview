#include "Pch.h"
#include "Input.h"

#include "GLFW/glfw3.h"
#include "Application/Application.h"
#include "Core/Window/Window.h"

namespace Brickview
{

	bool Input::isKeyPressed(int keyCode)
	{
		auto libWindow = (GLFWwindow*)Application::get()->getWindow()->getLibWindow();
		int state = glfwGetKey(libWindow, keyCode);

		return state == GLFW_PRESS;
	}

	bool Input::isMouseButtonPressed(int button)
	{
		auto libWindow = (GLFWwindow*)Application::get()->getWindow()->getLibWindow();
		int state = glfwGetMouseButton(libWindow, button);

		return state == GLFW_PRESS;
	}

}