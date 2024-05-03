#include "ApplicationLayer.h"

#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Brickview
{

	ApplicationLayer::ApplicationLayer()
	{
		//Renderer::init();
		RenderCommand::init();
		RenderedRenderer::init();
		SolidRenderer::init();
	}

	ApplicationLayer::~ApplicationLayer()
	{
		//Renderer::shutdown();
		RenderedRenderer::shutdown();
		SolidRenderer::shutdown();
	}

	void ApplicationLayer::onAttach()
	{
		m_legoPieceMesh = Mesh::load("data/models/brick.obj");
		m_planeMesh = Mesh::load("data/models/plane.obj");

		// Piece 1
		m_legoPiecePosition1 = { -1.0f, 0.0f, 0.0f };
		m_legoPieceMaterial1.Color = { 0.8f, 0.2f, 0.1f };

		// Piece 2
		m_legoPiecePosition2 = { 1.0f, 0.0f, 0.0f };
		m_legoPieceMaterial2.Color = { 0.2f, 0.2f, 0.8f };

		m_cameraControl = CameraController();

		m_light.Position = { 0.0f, 1.5f, 0.0f };
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
		RenderCommand::setViewportDimension(0, 0, e.getWidth(), e.getHeight());
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
			m_cameraControl.setTargetPoint(m_legoPiecePosition1);
		}
		return true;
	}

	void ApplicationLayer::onUpdate(float dt)
	{
		m_dt = dt;

		RenderCommand::setClearColor(0.2f, 0.2f, 0.2f);
		RenderCommand::clear();
		if (m_solidView)
		{
			SolidRenderer::begin(m_cameraControl.getCamera(), m_light);

			auto transform1 = glm::translate(glm::mat4(1.0), m_legoPiecePosition1);
			SolidRenderer::submitMesh(m_legoPieceMesh, transform1);
			auto transform2 = glm::translate(glm::mat4(1.0), m_legoPiecePosition2);
			SolidRenderer::submitMesh(m_legoPieceMesh, transform2);

			SolidRenderer::end();
		}
		else
		{
			RenderedRenderer::begin(m_cameraControl.getCamera(), m_light);

			auto transform1 = glm::translate(glm::mat4(1.0), m_legoPiecePosition1);
			RenderedRenderer::submitMesh(m_legoPieceMesh, m_legoPieceMaterial1, transform1);
			auto transform2 = glm::translate(glm::mat4(1.0), m_legoPiecePosition2);
			RenderedRenderer::submitMesh(m_legoPieceMesh, m_legoPieceMaterial2, transform2);

			RenderedRenderer::end();
		}
	}

	void ApplicationLayer::onGuiRender()
	{
		ImGui::Begin("Scene:");
		ImGui::SeparatorText("Elements:");

		// Light
		ImGui::SliderFloat3("Light Position", (float*)glm::value_ptr(m_light.Position), -5.0f, 5.0f);
		ImGui::ColorEdit3("Light Color", (float*)glm::value_ptr(m_light.Color));
		ImGui::Separator();

		// Lego Piece 1
		ImGui::SliderFloat3("Lego Piece Position 1", (float*)glm::value_ptr(m_legoPiecePosition1), -5.0f, 5.0f);
		ImGui::ColorEdit3("Lego Piece Color 1", (float*)glm::value_ptr(m_legoPieceMaterial1.Color));

		// Lego Piece 1
		ImGui::SliderFloat3("Lego Piece Position 2", (float*)glm::value_ptr(m_legoPiecePosition2), -5.0f, 5.0f);
		ImGui::ColorEdit3("Lego Piece Color 2", (float*)glm::value_ptr(m_legoPieceMaterial2.Color));

		// Render Type
		ImGui::SeparatorText("Render Settings:");
		//static bool drawLights = false;
		//if (ImGui::Checkbox("Draw lights", &drawLights))
		//	RenderedRenderer::drawLights(drawLights);

		ImGui::Checkbox("Solid view", &m_solidView);

		ImGui::End();

		ImGui::Begin("Render statistics");

		ImGui::Text("ts: %.3f ms", m_dt * 1000.0f);
		ImGui::Text("Fps: %.3f", m_dt == 0.0f ? 0.0f : 1.0f / m_dt);

		//ImGui::Text("Draw calls: %i", RenderedRenderer::getStats().DrawCalls);
		//ImGui::Text("Mesh vertex count: %i", RenderedRenderer::getStats().MeshVertexCount);
		//ImGui::Text("Mesh index count: %i", RenderedRenderer::getStats().MeshIndicesCount);

		ImGui::End();
	}

}