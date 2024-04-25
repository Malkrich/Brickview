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
#include "Renderer/Buffer/Buffer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer/RenderedRenderer.h"

namespace Brickview
{
	ApplicationLayer::ApplicationLayer()
	{
		RenderCommand::initialise();
		RenderedRenderer::init();
	}

	ApplicationLayer::~ApplicationLayer()
	{
		RenderedRenderer::shutdown();
	}

	void ApplicationLayer::onAttach()
	{
		m_legoPieceMesh = Mesh::load("data/models/brick.obj");
		m_legoPieceMaterial.Color = { 0.8f, 0.2f, 0.1f };

		m_cameraControl = CameraController();

		m_light.Position = { 0.0f, 0.0f, 0.0f };
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
			m_cameraControl.setTargetPoint(m_legoPiecePosition);
		}
		return true;
	}

	void ApplicationLayer::onUpdate(float dt)
	{
		RenderCommand::setClearColor(0.2f, 0.2f, 0.2f);
		RenderCommand::clear();

		RenderedRenderer::begin(m_cameraControl.getCamera(), m_light);

		auto transform1 = glm::translate(glm::mat4(1.0), m_legoPiecePosition);
		RenderedRenderer::submitMesh(m_legoPieceMesh, m_legoPieceMaterial, transform1);
		//auto transform2 = glm::translate(glm::mat4(1.0), m_legoPiecePosition + glm::vec3(1.0f, 0.0f, 0.0f));
		//auto cubeMesh = Mesh::load("data/models/cube.obj");
		//RenderedRenderer::submitMesh(cubeMesh, m_legoPieceMaterial, transform2);

		RenderedRenderer::end();
	}

	void ApplicationLayer::onGuiRender()
	{
		ImGui::Begin("Scene:");
		ImGui::SeparatorText("Elements:");

		// Light
		ImGui::SliderFloat3("Light Position", (float*)glm::value_ptr(m_light.Position), -5.0f, 5.0f);
		ImGui::ColorEdit3("Light Color", (float*)glm::value_ptr(m_light.Color));
		ImGui::Separator();

		// Lego Piece
		ImGui::SliderFloat3("Lego Piece Position", (float*)glm::value_ptr(m_legoPiecePosition), -5.0f, 5.0f);
		ImGui::SliderFloat3("Lego Piece Scale", (float*)glm::value_ptr(m_legoPieceScale), 0.0f, 1.0f);
		ImGui::ColorEdit3("Lego Piece Color", (float*)glm::value_ptr(m_legoPieceMaterial.Color));

		// Render Type
		ImGui::SeparatorText("Render Settings:");
		static bool drawLights = false;
		if (ImGui::Checkbox("Draw lights", &drawLights))
			RenderedRenderer::drawLights(drawLights);

		ImGui::End();
	}

}