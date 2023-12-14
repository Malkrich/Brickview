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

	private:
		float m_time = 0.0f;
	};
}