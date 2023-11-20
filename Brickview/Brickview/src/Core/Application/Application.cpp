#include "Pch.h"
#include "Application.h"

#include <glad/glad.h>

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

		// initialize Application attributes
		initialize();

		// Adding layers
		Layer* applicationLayer = new ApplicationLayer();
		m_layerStack->pushLayer(applicationLayer);
	}

	Application::~Application()
	{
	}

	void Application::initialize()
	{
		Window::WindowSettings windowSettings;
		windowSettings.name = "Brickview";
		m_window.reset(new Window(windowSettings));

		m_window->setEventCallbackFunction(BV_BIND_EVENT_FUNCTION(Application::onEvent));

		int version = gladLoadGL();
		if (version == 0)
		{
			BV_LOG_ERROR("Failed to initialize OpenGL context !");
			return;
		}

		m_layerStack.reset(new LayerStack());
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