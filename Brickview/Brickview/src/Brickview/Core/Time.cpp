#include "Pch.h"
#include "Time.h"

#include <GLFW/glfw3.h>

namespace Brickview
{

	float Time::getTime()
	{
		return (float)glfwGetTime();
	}

}