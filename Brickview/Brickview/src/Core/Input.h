#pragma once

namespace Brickview
{

	class Input
	{
	public:
		static bool isKeyPressed(int keyCode);
		static bool isMouseButtonPressed(int mouseButton);

		static glm::ivec2 getWindowSize();
	};

}