#include "ApplicationLayer.h"

#include <imgui.h>
#include <ImGuizmo/ImGuizmo.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <format>

namespace Brickview
{

	ApplicationLayer::ApplicationLayer()
		: m_viewportWidth(Input::getWindowSize().x)
		, m_viewportHeight(Input::getWindowSize().y)
	{
	}

	ApplicationLayer::~ApplicationLayer()
	{
	}

	void ApplicationLayer::onAttach()
	{
		// Scene
		m_scene = createRef<Scene>();
		m_selectedEntity = m_scene->createEntity();
		// Note: think about the dimensions, this is the window size,
		// not the actual ImGui viewport size
	
		// Renderer
		m_renderer = createRef<SceneRenderer>(m_viewportWidth, m_viewportHeight);
		// Editor camera
		CameraControllerSpecifications cameraControlSpecs;
		cameraControlSpecs.Width = (float)m_viewportWidth;
		cameraControlSpecs.Height = (float)m_viewportHeight;
		cameraControlSpecs.DistanceFromObject = 0.2f;
		cameraControlSpecs.CameraPosition = { 0.0f, 0.0f, cameraControlSpecs.DistanceFromObject };
		cameraControlSpecs.LaptopMode = m_laptopMode;
		m_cameraControl = createScope<CameraController>(cameraControlSpecs);

		// Panels
		m_legoPartsExplorerPanel = createScope<LegoPartsExplorerPanel>("./data/LDraw/parts/");
		m_legoPartsExplorerPanel->setOnLoadCallbackFunction([&](const std::filesystem::path& filePath)
		{
			LegoPartID partID = filePath.stem().string();
			Ref<Mesh> mesh = Mesh::load(filePath);
			m_scene->createLegoPartEntity(partID, mesh);
		});
		m_legoPartsCollectionPanel = createScope<LegoPartsCollectionPanel>(m_scene);
	}

	void ApplicationLayer::onDetach()
	{
	}

	void ApplicationLayer::onEvent(Event& e)
	{
		m_cameraControl->onEvent(e);

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
#if 0
		if (e.getKeyCode() == BV_KEY_KP_0)
		{
			m_cameraControl->setTargetPoint({ 0.0f, 0.0f, 0.0f });
		}
#endif
		return true;
	}

	void ApplicationLayer::onUpdate(DeltaTime dt)
	{
		m_dt = dt;

		m_renderer->resizeViewport(m_viewportWidth, m_viewportHeight);
		m_cameraControl->resize((float)m_viewportWidth, (float)m_viewportHeight);

		const PerspectiveCamera& camera = m_cameraControl->getCamera();
		m_renderer->begin(camera);
		m_scene->onUpdate(dt, m_renderer);
		m_renderer->render();
	}

	void ApplicationLayer::onGuiRender()
	{
		beginDockspace();

		// Menu
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Mode"))
			{
				if (ImGui::MenuItem("Laptop mode", nullptr, m_laptopMode))
				{
					m_laptopMode = !m_laptopMode;
					m_cameraControl->setLaptopMode(m_laptopMode);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGui::Begin("Viewport");
		// Updates
		m_cameraControl->setViewportHovered(ImGui::IsWindowHovered());
		ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
		ImVec2 viewportDim = ImGui::GetContentRegionAvail();
		ImVec2 viewportPos = ImGui::GetWindowPos();
		ImVec2 mousePosition = ImGui::GetMousePos();
		// viewportMinRegion essentially gives the tab bar dimensions if open 
		// (otherwise viewportMinRegion = (0, 0))
		mousePosition.x -= viewportPos.x + viewportMinRegion.x;
		mousePosition.y -= viewportPos.y + viewportMinRegion.y;
		// Flipping Y coordinate to make the bottom left corner (0, 0)
		mousePosition.y = viewportDim.y - mousePosition.y;

		if (mousePosition.x > 0 && mousePosition.x < viewportDim.x
			&& mousePosition.y > 0 && mousePosition.y < viewportDim.y)
		{
			int32_t pixelData = m_renderer->getEntityIDAt((uint32_t)mousePosition.x, (uint32_t)mousePosition.y);

			//BV_LOG_INFO("Mouse pos: {}, {}, pixel data: {}", mousePosition.x, mousePosition.y, pixelData);
		}

		// Guizmo
		if (m_selectedEntity)
		{
			const PerspectiveCamera& camera = m_cameraControl->getCamera();
			glm::mat4 objectTransform = m_selectedEntity.getComponent<TransformComponent>().getTransform();
			ImGuizmo::BeginFrame();
			ImGuizmo::SetRect(viewportPos.x + viewportMinRegion.x, viewportPos.y + viewportMinRegion.y, viewportDim.x, viewportDim.y);
			ImGuizmo::Manipulate(glm::value_ptr(camera.getViewMatrix()), glm::value_ptr(camera.getProjectionMatrix()),
				ImGuizmo::TRANSLATE, ImGuizmo::WORLD,
				glm::value_ptr(objectTransform));
		}

		m_viewportWidth = (uint32_t)viewportDim.x;
		m_viewportHeight = (uint32_t)viewportDim.y;
		// Render
		ImGui::Image((void*)m_renderer->getSceneRenderAttachment(), viewportDim, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

		ImGui::End(); // Viewport
		ImGui::PopStyleVar(3);

		// Shader lib
		ImGui::Begin("Shaders");

		const Ref<ShaderLibrary>& shaderLibrary = Renderer::getShaderLibrary();
		for (const auto& [name, shaderData] : *shaderLibrary)
		{
			ImGui::Text("%s", name.c_str());
			ImGui::SameLine();
			ImGui::PushID(name.c_str());
			if (ImGui::Button("Reload"))
			{
				std::filesystem::path shaderFilePath = shaderData.FilePath;
				shaderData.Shader->reload(shaderFilePath);
			}
			ImGui::PopID();
		}

		ImGui::End();

		ImGui::Begin("Renderer");

		ImGui::SeparatorText("Render statistics:");
		ImGui::Text("ts: %.3f ms", m_dt.getMilliseconds());
		ImGui::Text("Fps: %.3f", m_dt.getSeconds() == 0.0f ? 0.0f : 1.0f / m_dt.getSeconds());
		
#if 0
		const RenderStatistics& renderStats = Lego3DRenderer::getRenderStatistics();
		ImGui::Text("Draw calls: %i", renderStats.DrawCalls);
		ImGui::Text("Max Instance count: %i", renderStats.MaxInstanceCount);
#endif

		ImGui::End();

		m_legoPartsExplorerPanel->onGuiRender();
		m_legoPartsCollectionPanel->onGuiRender();

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
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
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
		ImGui::Begin("Brickview dockspace", &dockspaceOpen, window_flags);
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
			ImGuiID dockspace_id = ImGui::GetID("BrickviewDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;
	}

	void ApplicationLayer::endDockspace()
	{
		ImGui::End();
	}
}