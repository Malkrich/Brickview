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
		FrameBufferSpecifications fbSpec;
		fbSpec.Width = Input::getWindowSize().x;
		fbSpec.Height = Input::getWindowSize().y;
		m_frameBuffer = createScope<FrameBuffer>(fbSpec);

		m_legoPieceMesh = Mesh::load("data/Models/Brick.obj");
		m_planeMesh = Mesh::load("data/Models/Plane.obj");

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
		//RenderCommand::setViewportDimension(0, 0, e.getWidth(), e.getHeight());
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

		m_frameBuffer->bind();

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
		
		m_frameBuffer->unbind();
	}

	void ApplicationLayer::onGuiRender()
	{
		beginDockspace();

		ImGui::Begin("Scene:");
		ImGui::SeparatorText("Scene hierarchy:");

		// Light
		ImGui::SliderFloat3("Light Position", (float*)glm::value_ptr(m_light.Position), -5.0f, 5.0f);
		ImGui::ColorEdit3("Light Color", (float*)glm::value_ptr(m_light.Color));
		ImGui::Separator();

		// Lego Piece 1
		ImGui::SliderFloat3("Lego Piece Position 1", (float*)glm::value_ptr(m_legoPiecePosition1), -5.0f, 5.0f);
		ImGui::ColorEdit3("Lego Piece Color 1", (float*)glm::value_ptr(m_legoPieceMaterial1.Color));
		ImGui::Separator();

		// Lego Piece 1
		ImGui::SliderFloat3("Lego Piece Position 2", (float*)glm::value_ptr(m_legoPiecePosition2), -5.0f, 5.0f);
		ImGui::ColorEdit3("Lego Piece Color 2", (float*)glm::value_ptr(m_legoPieceMaterial2.Color));
		ImGui::Separator();

		ImGui::End();

		ImGui::Begin("Renderer");

		// Render Type
		ImGui::SeparatorText("Render Settings:");
		static bool drawLights = false;
		if (ImGui::Checkbox("Draw lights", &drawLights))
			RenderedRenderer::drawLights(drawLights);
		ImGui::Checkbox("Solid view", &m_solidView);

		ImGui::SeparatorText("Render statistics:");
		ImGui::Text("ts: %.3f ms", m_dt * 1000.0f);
		ImGui::Text("Fps: %.3f", m_dt == 0.0f ? 0.0f : 1.0f / m_dt);

		ImGui::Text("Draw calls: %i", RenderedRenderer::getStats().DrawCalls);
		ImGui::Text("Mesh vertex count: %i", RenderedRenderer::getStats().MeshVertexCount);
		ImGui::Text("Mesh index count: %i", RenderedRenderer::getStats().MeshIndicesCount);

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ));
		ImGui::Begin("Viewport");

		bool hovered = ImGui::IsWindowHovered();
		m_cameraControl.setViewportHovered(hovered);

		// Resize
		ImVec2 newViewportDim = ImGui::GetContentRegionAvail();
		if (m_viewportDim.x != newViewportDim.x || m_viewportDim.y != newViewportDim.y)
		{
			m_viewportDim.x = (uint32_t)newViewportDim.x;
			m_viewportDim.y = (uint32_t)newViewportDim.y;
			m_frameBuffer->resize(m_viewportDim.x, m_viewportDim.y);
			m_cameraControl.resize(m_viewportDim.x, m_viewportDim.y);
		}
		ImVec2 imSize = { (float)m_viewportDim.x, (float)m_viewportDim.y };
		uint32_t textureID = m_frameBuffer->getColorAttachment();
		ImGui::Image((void*)textureID, imSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::End();
		ImGui::PopStyleVar();

		endDockspace();
	}

	void ApplicationLayer::beginDockspace()
	{
		// From: https://github.com/TheCherno/Hazel/blob/master/Hazelnut/src/EditorLayer.cpp
		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = /*ImGuiWindowFlags_MenuBar |*/ ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 300.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;
	}

	void ApplicationLayer::endDockspace()
	{
		ImGui::End();
	}
}