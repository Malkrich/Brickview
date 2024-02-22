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

	glm::ivec2 Input::getWindowSize()
	{
		glm::ivec2 dimension;

		auto libWindow = (GLFWwindow*)Application::get()->getWindow()->getLibWindow();
		glfwGetWindowSize(libWindow, &dimension.x, &dimension.y);

		return dimension;
	}

}