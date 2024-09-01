#pragma once

#include "Core/Event/Event.h"

namespace Brickview
{

	class Layer
	{
	public:
		virtual ~Layer() = default;

		virtual void onAttach() = 0;
		virtual void onDetach() = 0;

		virtual void onEvent(Event& e) = 0;
		virtual void onUpdate(float dt) = 0;
		virtual void onGuiRender() = 0;
	};

}