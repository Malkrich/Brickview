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

// Renderer
#include "Renderer/Buffer/Layout.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer.h"
#include "Renderer/LegoRenderer.h"

namespace Brickview
{
	ApplicationLayer::~ApplicationLayer()
	{
		LegoRenderer::shutdown();
	}

	void ApplicationLayer::onAttach()
	{
		LegoRenderer::init();

	#if 0
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
		// COLORED QUAD
	#endif

		m_legoPieceMesh = Mesh::load("data/models/brick.obj");
		m_legoPieceMaterial.Color = { 0.8f, 0.2f, 0.1f };

		m_planeMesh = Mesh::load("data/models/plane.obj");
		m_planeMaterial.Color = { 0.1f, 0.2f, 0.8f };

		m_cameraControl = CameraController();

		m_light.Position = { 2.0f, 2.0f, 0.0f };
		m_light.Color = { 1.0f, 1.0f, 1.0f };
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
			m_cameraControl.setTargetPoint(m_legoPiecePosition);
		}
		return true;
	}

	void ApplicationLayer::onUpdate(float dt)
	{
		RenderCommand::setClearColor(0.2f, 0.2f, 0.2f);
		RenderCommand::clear();

		Renderer::begin(m_cameraControl.getCamera(), m_light);
		LegoRenderer::drawLight();

		// Lego Piece
		
		glm::mat4 transform;
		const unsigned int length = 2;
		for (unsigned int i = 0; i < length; i++)
		{
			for (unsigned int j = 0; j < length; j++)
			{
				transform = glm::translate(glm::mat4(1.0f), m_legoPiecePosition 
					+ (float)i*glm::vec3(1.0f, 0.0f, 0.0f)
					+ (float)j*glm::vec3(0.0f, 0.0f, 1.0f))
					* glm::scale(glm::mat4(1.0f), m_legoPieceScale);

				LegoRenderer::drawPiece(m_legoPieceMesh, m_legoPieceMaterial, transform);
			}
		}

		// Plane
		transform = glm::translate(glm::mat4(1.0f), m_planePosition)
			* glm::scale(glm::mat4(1.0f), m_planeScale);
		LegoRenderer::drawPiece(m_planeMesh, m_planeMaterial, transform);

		Renderer::end();
	}

	void ApplicationLayer::onGuiRender()
	{
		ImGui::Begin("Scene:");
		ImGui::Separator();

		// Light
		ImGui::SliderFloat3("Light Position", (float*)glm::value_ptr(m_light.Position), -5.0f, 5.0f);
		ImGui::ColorEdit3("Light Color", (float*)glm::value_ptr(m_light.Color));
		ImGui::Separator();

		// Lego Piece
		ImGui::SliderFloat3("Lego Piece Position", (float*)glm::value_ptr(m_legoPiecePosition), -5.0f, 5.0f);
		ImGui::SliderFloat3("Lego Piece Scale", (float*)glm::value_ptr(m_legoPieceScale), 0.0f, 1.0f);
		ImGui::ColorEdit3("Lego Piece Color", (float*)glm::value_ptr(m_legoPieceMaterial.Color));

		// Plane
		ImGui::SliderFloat3("Plane Position", (float*)glm::value_ptr(m_planePosition), -5.0f, 5.0f);
		ImGui::SliderFloat3("Plane Scale", (float*)glm::value_ptr(m_planeScale), 0.0f, 1.0f);
		ImGui::ColorEdit3("Plane Color", (float*)glm::value_ptr(m_planeMaterial.Color));

		ImGui::End();
	}

}