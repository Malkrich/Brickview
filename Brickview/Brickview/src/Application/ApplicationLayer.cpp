#include "Pch.h"
#include "ApplicationLayer.h"

#include "Core/Core.h"
#include "Core/Log.h"

#include <glad/glad.h>
#include <imgui.h>

namespace Brickview
{
	ApplicationLayer::~ApplicationLayer()
	{
	}

	void ApplicationLayer::onAttach()
	{
		m_colorShader.reset(new Shader("data/color.vs", "data/color.fs"));

		// Vertex buffer and index buffer
		float positions[] = { -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
							   0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
							   0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
							  -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
							   1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f };

		unsigned int indices[] = { 0, 1, 2,
								   2, 3, 0,
								   1, 4, 2 };

		m_vertexArray.reset(new VertexArray());

		m_vertexBuffer.reset(new VertexBuffer(sizeof(positions), positions));
		m_vertexArray->addVertexBuffer(m_vertexBuffer);

		m_indexBuffer.reset(new IndexBuffer(sizeof(indices), indices));
		m_vertexArray->setIndexBuffer(m_indexBuffer);
	}
	
	void ApplicationLayer::onDetach()
	{

	}

	void ApplicationLayer::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.dispatch<MouseMoveEvent>(BV_BIND_EVENT_FUNCTION(ApplicationLayer::onMouseMove));
	}

	bool ApplicationLayer::onMouseMove(const MouseMoveEvent& e)
	{
		//BV_LOG_INFO("Mouse move event : {0},{1}", e.getPosX(), e.getPosY());
		return true;
	}

	void ApplicationLayer::onUpdate(float dt)
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		m_colorShader->bind();
		m_vertexArray->bind();
		glDrawElements(GL_TRIANGLES, m_vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, 0);
	}

	void ApplicationLayer::onGuiRender()
	{
		// Test
		ImGui::Begin("Window test");
		ImGui::Text("Hello there !");
		ImGui::End();
	}

}