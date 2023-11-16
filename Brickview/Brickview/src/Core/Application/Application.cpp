#include "Application.h"

#include <glad/glad.h>

#include "Core/Core.h"
#include "Core/Log.h"

namespace Brickview
{
	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		BV_ASSERT(!s_instance, "Application class is already instanciated !");
		s_instance = this;

		Window::WindowSettings windowSettings;
		windowSettings.name = "Brickview";
		m_window.reset(new Window(windowSettings));

		initialize();

		m_window->setEventCallbackFunction(BIND_EVENT_FUNCTION(Application::onEvent));
	}

	Application::~Application()
	{
	}

	void Application::initialize()
	{
		int version = gladLoadGL();
		if (version == 0)
		{
			BV_LOG_ERROR("Failed to initialize OpenGL context !");
			return;
		}
	}

	void Application::run()
	{
		m_colorShader.reset(new Shader("data/color.vs", "data/color.fs"));

		// Vertex buffer and index buffer
		float positions[] = { -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
							   0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
							   0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
							  -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
							   1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f};

		unsigned int indices[] = { 0, 1, 2,
								   2, 3, 0,
								   1, 4, 2};

		m_vertexArray.reset(new VertexArray());

		m_vertexBuffer.reset(new VertexBuffer(sizeof(positions), positions));
		m_vertexArray->addVertexBuffer(m_vertexBuffer);

		m_indexBuffer.reset(new IndexBuffer(sizeof(indices), indices));
		m_vertexArray->setIndexBuffer(m_indexBuffer);

		while(m_running)
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);
			m_colorShader->bind();
			m_vertexArray->bind();
			glDrawElements(GL_TRIANGLES, m_vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, 0);

			/* GOAL :
			Renderer::setClearColor(...);
			Renderer::clear();

			Renderer::beginScene(camera);

			Renderer::submit(shader, vertexArray);

			Renderer::endscene(); */

			m_window->onUpdate();
		}
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(Application::onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FUNCTION(Application::onWindowResize));
	}

	bool Application::onWindowClose(const WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}

	bool Application::onWindowResize(const WindowResizeEvent& e)
	{
		BV_LOG_TRACE("on window resize : {0}x{1}", e.getWidth(), e.getHeight());
		return true;
	}
}