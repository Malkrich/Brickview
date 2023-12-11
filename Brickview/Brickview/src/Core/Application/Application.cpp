#include "Pch.h"
#include "Application.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// Core
#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Input.h"
#include "Core/KeyCodes.h"

// Layers
#include "Core/Layer/Layer.h"
#include "Application/ApplicationLayer.h"

namespace Brickview
{
	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		BV_ASSERT(!s_instance, "Application class is already instanciated !");
		s_instance = this;

		// initialize Application
		initialize();
	}

	Application::~Application()
	{
	}

	void Application::initialize()
	{
		// Window
		Window::WindowSettings windowSettings;
		windowSettings.name = "Brickview";
		m_window.reset(new Window(windowSettings));
		m_window->setEventCallbackFunction(BV_BIND_EVENT_FUNCTION(Application::onEvent));

		// GUI
		m_guiRenderer.reset(new GuiRenderer());

		// Layers
		m_layerStack.reset(new LayerStack());
		Layer* applicationLayer = new ApplicationLayer();
		m_layerStack->pushLayer(applicationLayer);
	}

	void Application::run()
	{
		while(m_running)
		{
			float dt = 0.0f;

			/* GOAL :
			Renderer::setClearColor(...);
			Renderer::clear();

			Renderer::beginScene(camera);

			Renderer::submit(shader, vertexArray);

			Renderer::endscene(); */

			for(auto layer : *m_layerStack)
				layer->onUpdate(dt);

			m_guiRenderer->onNewFrame();
			// Test
			ImGui::Begin("Window test");
			ImGui::Text("Hello there !");
			ImGui::End();

			m_guiRenderer->onRender();

			m_window->onUpdate();
		}
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		for (auto it = m_layerStack->end(); it != m_layerStack->begin();)
		{
			if(e.isHandled())
				break;

			(*(--it))->onEvent(e);
		}

		dispatcher.dispatch<WindowCloseEvent>(BV_BIND_EVENT_FUNCTION(Application::onWindowClose));
	}

	bool Application::onWindowClose(const WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}

}