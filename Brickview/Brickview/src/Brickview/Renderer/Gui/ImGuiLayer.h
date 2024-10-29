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

		void setBlockEvent(bool blockEvents) { m_blockEvents = blockEvents; }

		void begin();
		void end();

	private:
		bool m_blockEvents = true;

		float m_time = 0.0f;
	};
}