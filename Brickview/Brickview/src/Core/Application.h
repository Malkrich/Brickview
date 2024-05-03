#pragma once

#include "Core/Window.h"
#include "Core/Event/ApplicationEvent.h"
#include "Core/Layer/LayerStack.h"
#include "Renderer/Gui/GuiRenderer.h"

namespace Brickview
{

	class Application
	{
	public:
		Application();

		~Application();

		static const Application* get() { return s_instance; }
		std::shared_ptr<Window> getWindow() const { return m_window; }

		void run();

		void pushLayer(Layer* layer) { m_layerStack->pushLayer(layer); }

	private:
		void initialize();

		void onEvent(Event& e);

		bool onWindowClose(const WindowCloseEvent& e);

	private:
		bool m_running = true;

		std::shared_ptr<Window> m_window;

		std::unique_ptr<LayerStack> m_layerStack;

		std::unique_ptr<GuiRenderer> m_guiRenderer;

		// singleton instance
		static Application* s_instance;

		float m_currentTime;
	};

	Application* createApplication();
}