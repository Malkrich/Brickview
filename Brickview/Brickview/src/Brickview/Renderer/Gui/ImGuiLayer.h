#pragma once

#include "Core/Layer/Layer.h"

namespace Brickview
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onEvent(Event& e) override;

		void begin();
		void end();

	private:
		float m_time = 0.0f;
	};
}