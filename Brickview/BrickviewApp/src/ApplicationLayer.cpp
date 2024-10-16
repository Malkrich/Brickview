#include "ApplicationLayer.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <format>

namespace Brickview
{

	ApplicationLayer::ApplicationLayer()
		: m_legoBrickMaterial()
		, m_light()
	{}

	ApplicationLayer::~ApplicationLayer()
	{}

	void ApplicationLayer::onAttach()
	{
		uint32_t width = Input::getWindowSize().x;
		uint32_t height = Input::getWindowSize().y;
		m_viewport = createScope<Viewport>(width, height);

		// Camera
		CameraControllerSpecifications cameraControlSpec;
		cameraControlSpec.DistanceFromObject = 0.2;
		m_cameraControl = CameraController(cameraControlSpec);
		m_cameraControl.setLaptopMode(m_laptopMode);

#ifdef DEV_BRANCH
		m_fileIndexOffset = (m_selectedMesh / m_maxDisplayableFiles) * m_maxDisplayableFiles;
		uint32_t index = 0;
		for (const auto& file : std::filesystem::directory_iterator(m_ldrawBaseDir))
		{
			if (index == m_selectedMesh)
			{
				m_legoBrick = Mesh::load(file);
				break;
			}
			index++;
		}
		m_legoBrickMaterial.Color = { 0.8f, 0.2f, 0.2f };
#else
		// lego mesh data
		m_legoBrick = Mesh::load(m_ldrawBaseDir / "1.dat");
#endif

		// Lego mesh transforms
		m_legoBrickTransforms.reserve(3);
		m_legoBrickTransforms.push_back(glm::mat4(1.0f));
		m_legoBrickTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(-0.1f, 0.0f, 0.0f)));
		m_legoBrickTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.0f, 0.0f)));
		m_legoBrickTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, 0.0f)));
		m_legoBrickTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.1f, 0.0f)));
		m_legoBrickTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.1f)));
		m_legoBrickTransforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.1f)));

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
			m_cameraControl.setTargetPoint({ 0.0f, 0.0f, 0.0f });
		}
		return true;
	}

	void ApplicationLayer::onUpdate(float dt)
	{
		m_dt = dt;

		m_viewport->beginFrame();

		Lego3DRenderer::begin(m_cameraControl.getCamera(), m_light);
		// Instanced base rendering
		Lego3DRenderer::drawMeshes(m_legoBrick, m_legoBrickMaterial, m_legoBrickTransforms);
		Lego3DRenderer::end();

		m_viewport->endFrame();
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
					m_cameraControl.setLaptopMode(m_laptopMode);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Viewport
		m_viewport->onGuiRender();
		if (m_viewport->hasSizeChanged())
			m_cameraControl.resize(m_viewport->getWidth(), m_viewport->getHeight());
		m_cameraControl.setViewportHovered(m_viewport->isHovered());

		// Shader lib
		ImGui::Begin("Shaders");

		const Ref<ShaderLibrary> shaderLibrary = Lego3DRenderer::getShaderLibrary();
		for (const auto& [name, shaderData] : *shaderLibrary)
		{
			ImGui::Text("%s", name.c_str());
			ImGui::SameLine();
			std::string buttonName = std::format("Reload##{}", name);
			if (ImGui::Button(buttonName.c_str()))
			{
				std::filesystem::path shaderFilePath = shaderData.FilePath;
				shaderData.Shader->reload(shaderFilePath);
			}
		}

		ImGui::End();

		// Scene hierarchy
		ImGui::Begin("Scene:", nullptr, ImGuiWindowFlags_NoNav);
		ImGui::SeparatorText("Scene hierarchy:");

		// Light
		ImGui::SliderFloat3("Light Position", (float*)glm::value_ptr(m_light.Position), -5.0f, 5.0f);
		ImGui::ColorEdit3("Light Color", (float*)glm::value_ptr(m_light.Color));
		ImGui::Separator();

#ifdef DEV_BRANCH
		// Lego files explorer
		ImGui::Text("Lego parts explorer");
		if (ImGui::Button("<"))
		{
			if (m_fileIndexOffset > 0)
				m_fileIndexOffset -= m_maxDisplayableFiles;
		}
		ImGui::SameLine();
		if (ImGui::Button(">"))
			m_fileIndexOffset += m_maxDisplayableFiles;

		uint32_t fileIndex = 0;
		bool selected = false;
		for (const auto& file : std::filesystem::directory_iterator(m_ldrawBaseDir))
		{
			if (fileIndex < m_fileIndexOffset)
			{
				fileIndex++;
				continue;
			}
			if (fileIndex > m_fileIndexOffset + m_maxDisplayableFiles - 1)
				break;

			const std::filesystem::path& filePath = file.path();
			std::string fileName = filePath.filename().string();

			selected = fileIndex == m_selectedMesh;
			std::string itemName = std::format("{} : {}", fileIndex, fileName);
			if (ImGui::Selectable(itemName.c_str(), selected))
			{
				m_selectedMesh = fileIndex;
				m_legoBrick = Mesh::load(filePath);
			}

			fileIndex++;
		}
#endif
		ImGui::End();

		ImGui::Begin("Renderer");

		ImGui::SeparatorText("Render statistics:");
		ImGui::Text("ts: %.3f ms", m_dt * 1000.0f);
		ImGui::Text("Fps: %.3f", m_dt == 0.0f ? 0.0f : 1.0f / m_dt);

		ImGui::End();

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