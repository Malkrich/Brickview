#pragma once

#include <memory>

#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Shader/Shader.h"
#include "Core/Window/Window.h"
#include "Core/Event/Event.h"

namespace Brickview
{

	class Application
	{
	public:
		Application();

		~Application();

		void run();

	private:
		void initialize();

		void onEvent(Event& e);

		bool onWindowClose(const WindowCloseEvent& e);
		bool onWindowResize(const WindowResizeEvent& e);

	private:
		bool m_running = true;

		std::unique_ptr<Window> m_window;

		std::shared_ptr<VertexArray> m_vertexArray;
		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
		std::shared_ptr<Shader> m_colorShader;

		// singleton instance
		static Application* s_instance;
	};
}