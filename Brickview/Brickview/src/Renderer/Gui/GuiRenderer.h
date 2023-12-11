#pragma once

namespace Brickview
{
	class GuiRenderer
	{
	public:
		GuiRenderer();
		~GuiRenderer();

		void onNewFrame();
		void onRender();
	};
}