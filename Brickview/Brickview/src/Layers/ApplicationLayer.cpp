#include "Pch.h"
#include "ApplicationLayer.h"

// Lib
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

// Core
#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Input.h"
#include "Core/KeyCodes.h"

#include "Models/Mesh.h"

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
		// COLORED QUAD
		m_colorShader.reset(new Shader("data/shaders/color.vs", "data/shaders/color.fs"));

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

		// LEGO PIECE
		m_pieceShader.reset(new Shader("data/shaders/legoPiece.vs", "data/shaders/legoPiece.fs"));


		std::shared_ptr<Mesh> legoMesh = Mesh::load("data/models/brick.obj");

		m_pieceVertexArray.reset(new VertexArray());

		m_pieceVertexBuffer.reset(new VertexBuffer(legoMesh->getVertexBufferElementCount() * sizeof(float), legoMesh->getVertexBufferData()));
		Layout pieceLayout = {
			{ "a_position", BufferElementType::Float3 },
			{ "a_normal", BufferElementType::Float3 }
		};
		m_pieceVertexBuffer->setBufferLayout(pieceLayout);
		m_pieceVertexArray->addVertexBuffer(m_pieceVertexBuffer);

		m_pieceIndexBuffer.reset(new IndexBuffer(legoMesh->getIndexBufferElementCount() * sizeof(unsigned int), legoMesh->getIndexBufferData()));
		m_pieceVertexArray->setIndexBuffer(m_pieceIndexBuffer);

		// Camera Controller
		m_cameraControl = CameraController();

		// CLEAR COLOR
		m_clearColor = glm::vec3(0.2f, 0.2f, 0.2f);
	}
	
	void ApplicationLayer::onDetach()
	{

	}

	void ApplicationLayer::onEvent(Event& e)
	{
		m_cameraControl.onEvent(e);

		EventDispatcher dispatcher(e);

		dispatcher.dispatch<WindowResizeEvent>(BV_BIND_EVENT_FUNCTION(ApplicationLayer::onWindowResize));
		dispatcher.dispatch<MouseMovedEvent>(BV_BIND_EVENT_FUNCTION(ApplicationLayer::onMouseMoved));
		dispatcher.dispatch<KeyPressedEvent>(BV_BIND_EVENT_FUNCTION(ApplicationLayer::onKeyPressed));
	}

	bool ApplicationLayer::onWindowResize(const WindowResizeEvent& e)
	{
		Renderer::onWindowResize(e.getWidth(), e.getHeight());
		return true;
	}

	bool ApplicationLayer::onMouseMoved(const MouseMovedEvent& e)
	{
		//BV_LOG_INFO("Mouse move event : {0},{1}", e.getPosX(), e.getPosY());
		return true;
	}

	bool ApplicationLayer::onKeyPressed(const KeyPressedEvent& e)
	{
		if (e.getKeyCode() == BV_KEY_KP_0)
		{
			m_cameraControl.setTargetPoint(m_quadPosition);
		}
		return true;
	}

	void ApplicationLayer::onUpdate(float dt)
	{
		RenderCommand::setClearColor(m_clearColor);
		RenderCommand::clear();

		// TODO : Renderer::begin(camera);
		Renderer::begin(m_cameraControl.getCamera());

		glm::mat4 quadTransform = glm::translate(glm::mat4(1.0f), m_quadPosition) 
			* glm::scale(glm::mat4(1.0f), m_quadScale);
		// TODO : LegoRenderer::submit(legoPiece);
		Renderer::submit(m_pieceShader, m_pieceVertexArray, quadTransform);

		Renderer::end();
	}

	void ApplicationLayer::onGuiRender()
	{
		ImGui::Begin("Render Settings :");
		ImGui::Text("Clear color :");
		ImGui::SameLine();
		ImGui::ColorEdit3("Object color", glm::value_ptr(m_clearColor), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

		// Quad
		ImGui::Separator();
		ImGui::SliderFloat3("Quad position", (float*)glm::value_ptr(m_quadPosition), -5.0f, 5.0f);
		ImGui::SliderFloat3("Quad scale", (float*)glm::value_ptr(m_quadScale), 0.0f, 1.0f);

		ImGui::End();
	}

}