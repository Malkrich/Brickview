#include "Pch.h"
#include "LayerStack.h"

namespace Brickview
{
	LayerStack::LayerStack()
		: m_layerStack{}
	{}

	LayerStack::~LayerStack()
	{
		for(auto layer : m_layerStack)
			delete layer;
	}

	void LayerStack::pushLayer(Layer* layer)
	{
		m_layerStack.emplace(m_layerStack.begin() + m_currentLayerPushIndex, layer);
		layer->onAttach();
	}

	void LayerStack::pushOverlay(Layer* overlay)
	{
		m_layerStack.push_back(overlay);
		overlay->onAttach();
	}

}