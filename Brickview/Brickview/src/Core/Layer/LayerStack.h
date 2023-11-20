#pragma once

#include "Layer.h"

namespace Brickview
{

	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void pushLayer(Layer* layer);

		std::vector<Layer*>::iterator begin() { return m_layerStack.begin(); }
		std::vector<Layer*>::iterator end() { return m_layerStack.end(); }

	private:
		std::vector<Layer*> m_layerStack;
	};

}