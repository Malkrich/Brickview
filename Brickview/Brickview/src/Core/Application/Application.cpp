#include "Application.h"

#include "Core\Log.h"

#include "GL\glew.h"

#define BIND_EVENT_FUNCTION(function) std::bind(&function, this, std::placeholders::_1)

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
		if (glewInit() != GLEW_OK)
		{
			BV_LOG_ERROR("Glew init error !");
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
		EventDispatcher dispatcher;

		dispatcher.dispatch<WindowCloseEvent>(e, BIND_EVENT_FUNCTION(Application::onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(e, BIND_EVENT_FUNCTION(Application::onWindowResize));
	}

	void Application::onWindowClose(const WindowCloseEvent& e)
	{
		m_running = false;
	}

	void Application::onWindowResize(const WindowResizeEvent& e)
	{
		BV_LOG_TRACE("on window resize : {0}x{1}", e.getWidth(), e.getHeight());
	}
}