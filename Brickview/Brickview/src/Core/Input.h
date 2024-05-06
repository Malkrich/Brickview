#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

	class Input
	{
	public:
		static bool isKeyPressed(int32_t keyCode);
		static bool isMouseButtonPressed(int32_t mouseButton);

		/** Return the current window size from GLFW */
		static glm::ivec2 getWindowSize();
		/** Return the current mouse position from GLFW */
		static glm::ivec2 getMousePosition();
	};

}