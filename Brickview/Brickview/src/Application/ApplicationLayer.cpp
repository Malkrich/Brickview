#include "Pch.h"
#include "ApplicationLayer.h"

// Lib
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

// Core
#include "Core/Core.h"
#include "Core/Log.h"

// Renderer
#include "Renderer/Buffer/Layout.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer.h"

namespace Brickview
{
	ApplicationLayer::~ApplicationLayer()
	{
	}

	void ApplicationLayer::onAttach()
	{
		m_colorShader.reset(new Shader("data/color.vs", "data/color.fs"));

		// Vertex buffer and index buffer
		float positions[] = { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
							   0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
							   0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
							  -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };

		unsigned int indices[] = { 0, 1, 2,
								   2, 3, 0 };

		m_vertexArray.reset(new VertexArray());

		m_vertexBuffer.reset(new VertexBuffer(sizeof(positions), positions));
		Layout layout = { {"a_position", BufferElementType::Float3},
						  {"a_color", BufferElementType::Float4} };

		m_vertexBuffer->setBufferLayout(layout);
		m_vertexArray->addVertexBuffer(m_vertexBuffer);

		m_indexBuffer.reset(new IndexBuffer(sizeof(indices), indices));
		m_vertexArray->setIndexBuffer(m_indexBuffer);
		
		m_clearColor = glm::vec3(0.2f, 0.2f, 0.2f);
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
		BV_LOG_INFO("Clear color : {0}, {1}, {2}", m_clearColor.r, m_clearColor.g, m_clearColor.b);

		RenderCommand::setClearColor(m_clearColor);
		RenderCommand::clear();

		// TODO : Renderer::begin(camera);
		Renderer::begin();

		// TODO : LegoRenderer::submit(legoPiece);
		Renderer::submit(m_colorShader, m_vertexArray);
	}

	void ApplicationLayer::onGuiRender()
	{
		ImGui::Begin("Render Settings :");
		ImGui::Text("Clear color :");
		ImGui::SameLine();
		ImGui::ColorEdit3("Object color", glm::value_ptr(m_clearColor), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::End();
	}

}