#pragma once

#include "Core/Time.h"
#include "Core/Event/Event.h"

namespace Brickview
{

	class Layer
	{
	public:
		virtual ~Layer() = default;

		virtual void onAttach() {}
		virtual void onDetach() {}

		virtual void onEvent(Event& e) {}
		virtual void onUpdate(DeltaTime dt) {}
		virtual void onGuiRender() {}
	};

}