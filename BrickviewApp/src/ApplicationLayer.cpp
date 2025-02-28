#include "ApplicationLayer.h"

#include "Renderer/UniformBuffer.h"

#include <ImGuizmo/ImGuizmo.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <format>

namespace Brickview
{

	namespace ExampleScenes
	{

		void loadSpheresAndPlaneScene(Ref<Scene> scene)
		{
			float xStart = -1.0f;
			float xEnd = 1.0f;
			uint32_t sphereCount = 8;

			Ref<Mesh> smoothSphereMesh = Mesh::load("./data/Meshes/SmoothSphere.obj");
			smoothSphereMesh->scale(0.1f);
			Ref<Mesh> flatSphereMesh = Mesh::load("./data/Meshes/FlatSphere.obj");
			flatSphereMesh->scale(0.1f);

			// Plane ground
			Ref<Mesh> planeMesh = Mesh::load("./data/Meshes/Plane.obj");
			RendererMaterial planeMaterial;
			planeMaterial.Albedo = { 0.15f, 0.15f, 0.15f, 1.0f };
			planeMaterial.Roughness = 0.1f;
			scene->createMeshEntity(planeMesh, planeMaterial);

			RendererMaterial sphereMaterial;
			sphereMaterial.Albedo = { 1.0f, 0.0f, 0.0f, 1.0f };
			sphereMaterial.Roughness = 0.1f;
			sphereMaterial.Metalness = 0.0f;
			glm::vec3 position = { -0.3f, 0.3f, -0.5f };
			scene->createMeshEntity(smoothSphereMesh, position, sphereMaterial);

			sphereMaterial.Albedo = { 0.2f, 0.8f, 0.2f, 1.0f };
			sphereMaterial.Metalness = 1.0f;
			sphereMaterial.Roughness = 0.5f;
			position.x = 0.3f;
			scene->createMeshEntity(flatSphereMesh, position, sphereMaterial);

			const glm::vec3 white = glm::vec3(1.0f);
			const glm::vec3 green = { 0.0f, 1.0f, 0.0 };
			for (uint32_t i = 0; i < 4; i++)
			{
				float factor = (float)i / (4.0f - 1.0f);
				float xPos = factor * -0.5f + (1.0f - factor) * 0.5f;
				glm::vec3 lightPos = { 0.0f, 0.5f, 0.0f };
				glm::vec3 lightColor = white * (1.0f - factor) + green * factor;
				lightPos.x = xPos;
				scene->createLightEntity(lightPos, lightColor);
			}
		}

	}

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

		//ExampleScenes::loadSpheresAndPlaneScene(m_scene);

		// Renderer
		// Note: think about the dimensions, this is the native window size
		// not the actual ImGui viewport size
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
		m_scenePartsListPanel = createScope<ScenePartsListPanel>(m_scene);
		m_legoPartPropertiesPanel = createScope<LegoPartPropertiesPanel>();
	}

	void ApplicationLayer::onDetach()
	{
	}

	void ApplicationLayer::onEvent(Event& e)
	{
		m_cameraControl->onEvent(e);

		EventDispatcher dispatcher(e);

		dispatcher.dispatch<MousePressedEvent>(BV_BIND_EVENT_FUNCTION(ApplicationLayer::onMousePressed));
		dispatcher.dispatch<KeyPressedEvent>(BV_BIND_EVENT_FUNCTION(ApplicationLayer::onKeyPressed));
	}

	bool ApplicationLayer::onMousePressed(const MousePressedEvent& e)
	{
		if (e.getMouseButton() == BV_MOUSE_BUTTON_LEFT && !(ImGuizmo::IsOver() && m_gizmoVisible))
		{
			ImVec2 screenPosition = {
				m_mousePosition.x - m_viewportMinBound.x,
				m_mousePosition.y - m_viewportMinBound.y
			};
			// Flipping Y coordinate to make the bottom left corner (0, 0)
			float viewportHeight = m_viewportMaxBound.y - m_viewportMinBound.y;
			screenPosition.y = viewportHeight - screenPosition.y;

			int32_t entityID = m_renderer->getEntityIDAt((uint32_t)screenPosition.x, (uint32_t)screenPosition.y);
			Entity selectedEntity = entityID == -1 ? Entity() : Entity((entt::entity)entityID, m_scenePartsListPanel->getContext().get());
			m_legoPartPropertiesPanel->setEntityContext(selectedEntity);
			m_renderer->setSelectedEntity(selectedEntity);
		}

		return true;
	}

	bool ApplicationLayer::onKeyPressed(const KeyPressedEvent& e)
	{
		switch (e.getKeyCode())
		{
			case BV_KEY_ESCAPE:
				m_currentManipulationType = EditorManipulationType::None;
				break;
			case BV_KEY_T:
				m_currentManipulationType = EditorManipulationType::Translate;
				break;
			case BV_KEY_R:
				m_currentManipulationType = EditorManipulationType::Rotate;
				break;
			case BV_KEY_F:
				Entity selectedEntity = m_legoPartPropertiesPanel->getEntityContext();
				if (selectedEntity)
					onFocusEntity(selectedEntity);
				break;
		}

		return true;
	}

	void ApplicationLayer::onUpdate(DeltaTime dt)
	{
		m_dt = dt;

		m_renderer->resizeViewport(m_viewportWidth, m_viewportHeight);
		m_cameraControl->resize((float)m_viewportWidth, (float)m_viewportHeight);

		if (!ImGuizmo::IsOver() || !m_cameraControl->isLaptopModeEnable())
			m_cameraControl->onUpdate();

		const PerspectiveCamera& camera = m_cameraControl->getCamera();
		m_scene->onRender(camera, m_renderer);
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
		// Focus if middle mouse button clicked because it controls the viewport
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
			ImGui::SetWindowFocus("Viewport");

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
			ImGui::OpenPopup("rightClickViewportMenu");

		if (ImGui::BeginPopup("rightClickViewportMenu"))
		{
			if (ImGui::Button("Light"))
			{
				m_scene->createLightEntity();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		// Updates
		bool viewportActive = ImGui::IsWindowHovered() && ImGui::IsWindowFocused();
		Application::get()->getGuiLayer()->setBlockEvent(!viewportActive);

		// Resizing
		// Save current size before new size computation to display the current frame properly
		ImVec2 currentFrameViewportDim = { m_viewportMaxBound.x - m_viewportMinBound.x, m_viewportMaxBound.y - m_viewportMinBound.y };
		// Refresh resizing
		m_cameraControl->setViewportHovered(ImGui::IsWindowHovered());
		ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
		ImVec2 viewportDim = ImGui::GetContentRegionAvail();
		ImVec2 viewportPos = ImGui::GetWindowPos();
		m_mousePosition = ImGui::GetMousePos();
		// viewportMinRegion essentially gives the tab bar dimensions if open 
		// (otherwise viewportMinRegion = (0, 0))
		m_viewportMinBound = { viewportPos.x + viewportMinRegion.x, viewportPos.y + viewportMinRegion.y };
		m_viewportMaxBound = { m_viewportMinBound.x + viewportDim.x, m_viewportMinBound.y + viewportDim.y };
		m_viewportWidth = (uint32_t)viewportDim.x;
		m_viewportHeight = (uint32_t)viewportDim.y;

		// Render
		ImGui::Image((void*)m_renderer->getSceneRenderAttachment(), currentFrameViewportDim, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

		// Gizmo
		m_gizmoVisible = false;
		Entity selectedEntity = m_legoPartPropertiesPanel->getEntityContext();
		if (selectedEntity && m_currentManipulationType != EditorManipulationType::None)
		{
			m_gizmoVisible = true;
			// Window setup
			ImGuizmo::SetAlternativeWindow(ImGui::GetCurrentWindow());
			ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
			ImGuizmo::SetOrthographic(false);

			const PerspectiveCamera& camera = m_cameraControl->getCamera();
			TransformComponent& transform = selectedEntity.getComponent<TransformComponent>();
			glm::mat4 transformMatrix = transform.getTransform();

			ImGuizmo::SetRect(m_viewportMinBound.x, m_viewportMinBound.y, 
				m_viewportMaxBound.x - m_viewportMinBound.x, m_viewportMaxBound.y - m_viewportMinBound.y);

			ImGuizmo::OPERATION guizmoManip;
			switch (m_currentManipulationType)
			{
				case EditorManipulationType::Translate: guizmoManip = ImGuizmo::TRANSLATE; break;
				case EditorManipulationType::Rotate:    guizmoManip = ImGuizmo::ROTATE; break;
				default: BV_ASSERT(false, "Unknown manip type!"); break;
			}
			ImGuizmo::Manipulate(glm::value_ptr(camera.getViewMatrix()), glm::value_ptr(camera.getProjectionMatrix()),
				guizmoManip, ImGuizmo::LOCAL,
				glm::value_ptr(transformMatrix));

			glm::vec3 translation, rotation, scale;
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transformMatrix),
				glm::value_ptr(translation),
				glm::value_ptr(rotation),
				glm::value_ptr(scale));
			transform.Translation = translation;
			transform.Rotation = glm::radians(rotation);
		}

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
		
		ImGui::SeparatorText("Renderer Settings:");

		ImGui::Columns(2);
		{
			SceneRendererSettings& rendererSettings = m_renderer->getRendererSettings();

			// Render mode
			ImGui::Text("Renderer Type");
			ImGui::NextColumn();
			RendererType rendererType = rendererSettings.RendererType;
			const char* rendererTypeStrings[] = {"Solid", "Lighted Phong", "Lighted PBR"};
			const char* selectedRendererTypeString = rendererTypeStrings[(int32_t)rendererType];
			if (ImGui::BeginCombo("##rendererType", selectedRendererTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = selectedRendererTypeString == rendererTypeStrings[i];
					if (ImGui::Selectable(rendererTypeStrings[i], isSelected))
					{
						selectedRendererTypeString = rendererTypeStrings[i];
						rendererSettings.RendererType = (RendererType)i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			ImGui::NextColumn();

			// Selection outline width
			ImGui::Text("Outline width");
			ImGui::NextColumn();
			ImGui::SliderFloat("##outlineWidthSlider", &rendererSettings.OutlineWidth, 1.0f, 10.0f);
			ImGui::NextColumn();
		}
		ImGui::Columns(1);

		ImGui::End();

		m_legoPartsExplorerPanel->onGuiRender();
		m_scenePartsListPanel->onGuiRender();
		m_legoPartPropertiesPanel->onGuiRender();

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

	void ApplicationLayer::onFocusEntity(Entity entity)
	{
		TransformComponent& transform = entity.getComponent<TransformComponent>();
		m_cameraControl->setTargetPoint(transform.Translation);
	}

}