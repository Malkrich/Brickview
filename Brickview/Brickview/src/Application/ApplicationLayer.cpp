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
#include "Core/FileSystem/ObjLoader.h"

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

		std::vector<glm::vec3> piecePositions;
		std::vector<unsigned int> pieceIndices;
		ObjLoader::loadObj("data/models/brick.obj", piecePositions, pieceIndices);

		m_pieceVertexArray.reset(new VertexArray());

		m_pieceVertexBuffer.reset(new VertexBuffer(piecePositions.size() * 3 * sizeof(float), piecePositions.data()));
		Layout pieceLayout = { { "a_position", BufferElementType::Float3 } };
		m_pieceVertexBuffer->setBufferLayout(pieceLayout);
		m_pieceVertexArray->addVertexBuffer(m_pieceVertexBuffer);

		m_pieceIndexBuffer.reset(new IndexBuffer(pieceIndices.size() * sizeof(unsigned int), pieceIndices.data()));
		m_pieceVertexArray->setIndexBuffer(m_pieceIndexBuffer);

		// CLEAR COLOR
		m_clearColor = glm::vec3(0.2f, 0.2f, 0.2f);
	}
	
	void ApplicationLayer::onDetach()
	{

	}

	void ApplicationLayer::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.dispatch<WindowResizeEvent>(BV_BIND_EVENT_FUNCTION(ApplicationLayer::onWindowResize));
		dispatcher.dispatch<MouseMoveEvent>(BV_BIND_EVENT_FUNCTION(ApplicationLayer::onMouseMove));
	}

	bool ApplicationLayer::onWindowResize(const WindowResizeEvent& e)
	{
		m_camera.setViewportDimension(e.getWidth(), e.getHeight());
		Renderer::onWindowResize(e.getWidth(), e.getHeight());
		return true;
	}

	bool ApplicationLayer::onMouseMove(const MouseMoveEvent& e)
	{
		//BV_LOG_INFO("Mouse move event : {0},{1}", e.getPosX(), e.getPosY());
		return true;
	}

	void ApplicationLayer::onUpdate(float dt)
	{
		RenderCommand::setClearColor(m_clearColor);
		RenderCommand::clear();

		// TODO : Renderer::begin(camera);
		Renderer::begin(m_camera);

		glm::mat4 quadTransform = glm::translate(glm::mat4(1.0f), m_quadPosition) 
			* glm::scale(glm::mat4(1.0f), m_quadScale);
		// TODO : LegoRenderer::submit(legoPiece);
		//Renderer::submit(m_colorShader, m_vertexArray, quadTransform);
		Renderer::submit(m_pieceShader, m_pieceVertexArray, quadTransform);
	}

	void ApplicationLayer::onGuiRender()
	{
		ImGui::Begin("Render Settings :");
		ImGui::Text("Clear color :");
		ImGui::SameLine();
		ImGui::ColorEdit3("Object color", glm::value_ptr(m_clearColor), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

		// Camera
		ImGui::Separator();
		const glm::vec3& position = m_camera.getPosition();
		ImGui::SliderFloat3("Camera position", (float*)glm::value_ptr(position), -5.0f, 5.0f);
		m_camera.setPosition(position);
		const glm::vec3& rotation = m_camera.getRotation();
		ImGui::SliderFloat3("Camera rotation", (float*)glm::value_ptr(rotation), -90.0f, 90.0f);
		m_camera.setRotation(rotation);

		// Quad
		ImGui::Separator();
		ImGui::SliderFloat3("Quad position", (float*)glm::value_ptr(m_quadPosition), -5.0f, 5.0f);
		ImGui::SliderFloat3("Quad scale", (float*)glm::value_ptr(m_quadScale), 0.0f, 1.0f);

		ImGui::End();
	}

}