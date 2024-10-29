#pragma once

#include "Core/Window.h"
#include "Core/Event/ApplicationEvent.h"
#include "Core/Layer/LayerStack.h"
#include "Renderer/Gui/ImGuiLayer.h"

namespace Brickview
{

	class Application
	{
	public:
		Application();

		~Application();

		static const Application* get() { return s_instance; }
		Ref<Window> getWindow() const { return m_window; }
		ImGuiLayer* getGuiLayer() const { return m_ImGuiLayer; }

		void run();

		void pushLayer(Layer* layer) { m_layerStack->pushLayer(layer); }

	private:
		void initialize();

		void onEvent(Event& e);

		bool onWindowClose(const WindowCloseEvent& e);

	private:
		bool m_running = true;

		Ref<Window> m_window = nullptr;

		Scope<LayerStack> m_layerStack = nullptr;

		ImGuiLayer* m_ImGuiLayer = nullptr;

		// singleton instance
		static Application* s_instance;

		float m_currentTime;
	};

	Application* createApplication();
}