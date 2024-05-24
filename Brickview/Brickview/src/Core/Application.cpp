#include "Pch.h"
#include "Application.h"
// Core
#include "Core/Time.h"
#include "Core/Input.h"
#include "Core/KeyCodes.h"
// Layers
#include "Core/Layer/Layer.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

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
		windowSettings.Name = "Brickview";
		m_window = createRef<Window>(windowSettings);
		m_window->setEventCallbackFunction(BV_BIND_EVENT_FUNCTION(Application::onEvent));

		// GUI
		m_guiRenderer = createScope<GuiRenderer>();

		// Layers
		m_layerStack = createScope<LayerStack>();
	}

	void Application::run()
	{
		while(m_running)
		{
			float time = Time::getTime();
			float dt = time - m_currentTime;
			m_currentTime = time;

			for(auto layer : *m_layerStack)
				layer->onUpdate(dt);

			m_guiRenderer->onNewFrame();
			for (auto layer : *m_layerStack)
				layer->onGuiRender();
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