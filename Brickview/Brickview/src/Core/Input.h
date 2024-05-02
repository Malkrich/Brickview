#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

	class Input
	{
	public:
		static bool isKeyPressed(int keyCode);
		static bool isMouseButtonPressed(int mouseButton);

		/** Return the current window size from GLFW */
		static glm::ivec2 getWindowSize();
		/** Return the current mouse position from GLFW */
		static glm::ivec2 getMousePosition();
	};

}