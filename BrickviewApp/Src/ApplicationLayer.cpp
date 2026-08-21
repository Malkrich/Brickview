#include "ApplicationLayer.h"

#include "Lego/LegoMeshReader.h"
#include "Scene/BrickviewScene.h"

#include "Core/Application.h"
#include "Core/Input.h"
#include "Core/KeyCodes.h"
#include "Core/MouseButtons.h"
#include "IO/MeshImporter.h"
#include "Renderer/Renderer.h"

#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>

namespace Brickview
{

  namespace ExampleScenes
  {

    static void LoadSpheresAndPlaneScene(Soup::Ref<BrickviewScene> scene)
    {
      float xStart = -1.0f;
      float xEnd = 1.0f;
      uint32_t sphereCount = 8;

      Soup::Ref<Soup::Mesh> smoothSphereMesh = Soup::MeshImporter::Import("./data/Meshes/SmoothSphere.obj");
      smoothSphereMesh->Scale(0.1f);
      Soup::Ref<Soup::Mesh> flatSphereMesh = Soup::MeshImporter::Import("./data/Meshes/FlatSphere.obj");
      flatSphereMesh->Scale(0.1f);

      // Plane ground
      Soup::Ref<Soup::Mesh> planeMesh = Soup::MeshImporter::Import("./data/Meshes/Plane.obj");
      Soup::RendererMaterial planeMaterial;
      planeMaterial.Albedo = { 0.15f, 0.15f, 0.15f, 1.0f };
      planeMaterial.Roughness = 0.1f;
      scene->CreateMeshEntity(planeMesh, planeMaterial);

      Soup::RendererMaterial sphereMaterial;
      sphereMaterial.Albedo = { 1.0f, 0.0f, 0.0f, 1.0f };
      sphereMaterial.Roughness = 0.1f;
      sphereMaterial.Metalness = 0.0f;
      glm::vec3 position = { -0.3f, 0.3f, -0.5f };
      scene->CreateMeshEntity(smoothSphereMesh, position, sphereMaterial);

      sphereMaterial.Albedo = { 0.2f, 0.8f, 0.2f, 1.0f };
      sphereMaterial.Metalness = 1.0f;
      sphereMaterial.Roughness = 0.5f;
      position.x = 0.3f;
      scene->CreateMeshEntity(flatSphereMesh, position, sphereMaterial);

      const glm::vec3 white = glm::vec3(1.0f);
      const glm::vec3 green = { 0.0f, 1.0f, 0.0 };
      for (uint32_t i = 0; i < 4; i++)
      {
        float factor = (float)i / (4.0f - 1.0f);
        float xPos = factor * -0.5f + (1.0f - factor) * 0.5f;
        glm::vec3 lightPos = { 0.0f, 0.5f, 0.0f };
        glm::vec3 lightColor = white * (1.0f - factor) + green * factor;
        lightPos.x = xPos;
        scene->CreateLightEntity(lightPos, lightColor);
      }
    }

    static void LoadSphereOnlyScene(Soup::Ref<BrickviewScene> scene)
    {
      Soup::Ref<Soup::Mesh> smoothSphereMesh = Soup::MeshImporter::Import("./data/Meshes/SmoothSphere.obj");
      smoothSphereMesh->Scale(0.1f);

      Soup::RendererMaterial sphereMaterial;
      sphereMaterial.Albedo = { 1.0f, 0.0f, 0.0f, 1.0f };
      sphereMaterial.Roughness = 0.1f;
      sphereMaterial.Metalness = 0.0f;
      scene->CreateMeshEntity(smoothSphereMesh, glm::vec3(0.0f), sphereMaterial);
    }

    static void LoadPBRTestScene(Soup::Ref<BrickviewScene> scene)
    {
      Soup::Ref<Soup::Mesh> sphere = Soup::MeshImporter::Import("./data/Meshes/SmoothSphere.obj");
      sphere->Scale(0.1f);

      uint32_t rowCount = 5;
      uint32_t columnCount = 5;
      for (uint32_t i = 0; i < rowCount; i++)
      {
        float metallic = (float)i / (float)(rowCount - 1);
        for (uint32_t j = 0; j < columnCount; j++)
        {
          float roughness = (float)j / (float)(columnCount - 1);
          float x = -0.5f * (1.0f - roughness) + 0.5f * roughness;
          float y = -0.0f * (1.0f - metallic) + 1.0f * metallic;
          glm::vec3 position = { x, y, 0.0f };

          Soup::RendererMaterial material;
          material.Albedo = { 1.0f, 0.0f, 0.0f, 1.0f };
          material.Metalness = metallic;
          material.Roughness = roughness;
          scene->CreateMeshEntity(sphere, position, material);
        }
      }
      scene->CreateLightEntity({ 0.0f, 0.0f, -1.0f });
    }

  }

  ApplicationLayer::ApplicationLayer()
    : m_ViewportWidth(Soup::Input::GetWindowSize().x)
    , m_ViewportHeight(Soup::Input::GetWindowSize().y)
  {
  }

  void ApplicationLayer::OnAttach()
  {
    Soup::MeshImporter::AddReader<LegoMeshReader>(".dat");

    // Scene
    m_Scene = Soup::CreateRef<BrickviewScene>();

    // Renderer
    // Note: think about the dimensions, this is the native window size
    // not the actual ImGui viewport size
    m_Renderer = Soup::CreateRef<Soup::SceneRenderer>(m_ViewportWidth, m_ViewportHeight);

    // Editor camera
    Soup::CameraControllerSpecifications cameraControlSpecs;
    cameraControlSpecs.Width = (float)m_ViewportWidth;
    cameraControlSpecs.Height = (float)m_ViewportHeight;
    cameraControlSpecs.DistanceFromObject = 0.5f;
    cameraControlSpecs.CameraPosition = { 0.0f, 0.0f, cameraControlSpecs.DistanceFromObject };
    cameraControlSpecs.LaptopMode = m_LaptopMode;
    m_CameraControl = Soup::CreateScope<Soup::CameraController>(cameraControlSpecs);

    // Panels
    m_LegoPartsExplorerPanel = Soup::CreateScope<LegoPartsExplorerPanel>("./data/LDraw/parts/");
    m_LegoPartsExplorerPanel->SetOnLoadCallbackFunction(
      [&](const std::filesystem::path& filePath)
      {
        LegoPartID partID = filePath.stem().string();
        Soup::Ref<Soup::Mesh> mesh = Soup::MeshImporter::Import(filePath);
        m_Scene->CreateLegoPartEntity(partID, mesh);
      });
    m_ScenePartsListPanel = Soup::CreateScope<ScenePartsListPanel>(m_Scene);
    m_LegoPartPropertiesPanel = Soup::CreateScope<LegoPartPropertiesPanel>(m_Scene);

    m_HdriFilePath = "./data/HDRI/bambanani_sunset_2k.hdr";
    ImportHdriFile();
  }

  void ApplicationLayer::OnDetach() {}

  void ApplicationLayer::OnEvent(Soup::Event& e)
  {
    m_CameraControl->OnEvent(e);

    Soup::EventDispatcher dispatcher(e);

    dispatcher.Dispatch<Soup::MousePressedEvent>(SP_BIND_EVENT_FUNCTION(ApplicationLayer::OnMousePressed));
    dispatcher.Dispatch<Soup::KeyPressedEvent>(SP_BIND_EVENT_FUNCTION(ApplicationLayer::OnKeyPressed));
  }

  bool ApplicationLayer::OnMousePressed(const Soup::MousePressedEvent& e)
  {
    if (e.GetMouseButton() == Soup::MouseButton::Left && !(ImGuizmo::IsOver() && m_GizmoVisible))
    {
      ImVec2 screenPosition = { m_MousePosition.x - m_ViewportMinBound.x, m_MousePosition.y - m_ViewportMinBound.y };
      // Flipping Y coordinate to make the bottom left corner (0, 0)
      float viewportHeight = m_ViewportMaxBound.y - m_ViewportMinBound.y;
      screenPosition.y = viewportHeight - screenPosition.y;

      int32_t entityID = m_Renderer->GetEntityIDAt((uint32_t)screenPosition.x, (uint32_t)screenPosition.y);
      Soup::Entity selectedEntity = entityID == -1 ? Soup::Entity() : m_Scene->GetEntity(entityID);
      m_LegoPartPropertiesPanel->SetEntityContext(selectedEntity);
      m_Renderer->SetSelectedEntity(selectedEntity);
    }

    return true;
  }

  bool ApplicationLayer::OnKeyPressed(const Soup::KeyPressedEvent& e)
  {
    switch (e.GetKeyCode())
    {
      case Soup::KeyCode::Escape:
        m_CurrentManipulationType = EditorManipulationType::None;
        break;
      case Soup::KeyCode::T:
        m_CurrentManipulationType = EditorManipulationType::Translate;
        break;
      case Soup::KeyCode::R:
        m_CurrentManipulationType = EditorManipulationType::Rotate;
        break;
      case Soup::KeyCode::F:
      {
        Soup::Entity selectedEntity = m_LegoPartPropertiesPanel->GetEntityContext();
        if (selectedEntity)
          OnFocusEntity(selectedEntity);
        break;
      }
      default:
        break;
    }

    return true;
  }

  void ApplicationLayer::OnUpdate(Soup::DeltaTime dt)
  {
    m_Dt = dt;

    m_Renderer->ResizeViewport(m_ViewportWidth, m_ViewportHeight);
    m_CameraControl->Resize((float)m_ViewportWidth, (float)m_ViewportHeight);

    if (!ImGuizmo::IsOver())
      m_CameraControl->OnUpdate();

    const Soup::PerspectiveCamera& camera = m_CameraControl->GetCamera();
    m_Renderer->Render(camera, m_Scene);
  }

  void ApplicationLayer::OnGuiRender()
  {
    BeginDockspace();

    // Menu
    if (ImGui::BeginMenuBar())
    {
      if (ImGui::BeginMenu("Mode"))
      {
        if (ImGui::MenuItem("Laptop mode", nullptr, m_LaptopMode))
        {
          m_LaptopMode = !m_LaptopMode;
          m_CameraControl->SetLaptopMode(m_LaptopMode);
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
        m_Scene->CreateLightEntity();
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    // Updates
    bool viewportActive = ImGui::IsWindowHovered() && ImGui::IsWindowFocused();
    Soup::Application::GetInstance().GetGuiLayer()->SetBlockEvent(!viewportActive);

    // Resizing
    // Save current size before new size computation to display the current frame properly
    ImVec2 currentFrameViewportDim = { m_ViewportMaxBound.x - m_ViewportMinBound.x,
      m_ViewportMaxBound.y - m_ViewportMinBound.y };
    // Refresh resizing
    m_CameraControl->SetViewportHovered(ImGui::IsWindowHovered());
    ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
    ImVec2 viewportDim = ImGui::GetContentRegionAvail();
    ImVec2 viewportPos = ImGui::GetWindowPos();
    m_MousePosition = ImGui::GetMousePos();
    // viewportMinRegion essentially gives the tab bar dimensions if open
    // (otherwise viewportMinRegion = (0, 0))
    m_ViewportMinBound = { viewportPos.x + viewportMinRegion.x, viewportPos.y + viewportMinRegion.y };
    m_ViewportMaxBound = { m_ViewportMinBound.x + viewportDim.x, m_ViewportMinBound.y + viewportDim.y };
    m_ViewportWidth = (uint32_t)viewportDim.x;
    m_ViewportHeight = (uint32_t)viewportDim.y;

    // Render
    ImGui::Image((ImTextureID)((uint64_t)m_Renderer->GetRenderedImage()->GetHandle()), currentFrameViewportDim,
      ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

    // Gizmo
    m_GizmoVisible = false;
    if (m_LegoPartPropertiesPanel->HasEntityContext() && m_CurrentManipulationType != EditorManipulationType::None)
    {
      Soup::Entity selectedEntity = m_LegoPartPropertiesPanel->GetEntityContext();
      m_GizmoVisible = true;
      // Window setup
      ImGuizmo::SetAlternativeWindow(ImGui::GetCurrentWindow());
      ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
      ImGuizmo::SetOrthographic(false);

      const Soup::PerspectiveCamera& camera = m_CameraControl->GetCamera();
      Soup::TransformComponent& transform = selectedEntity.GetComponent<Soup::TransformComponent>();
      glm::mat4 transformMatrix = transform.GetTransform();

      ImGuizmo::SetRect(m_ViewportMinBound.x, m_ViewportMinBound.y, m_ViewportMaxBound.x - m_ViewportMinBound.x,
        m_ViewportMaxBound.y - m_ViewportMinBound.y);

      ImGuizmo::OPERATION guizmoManip;
      switch (m_CurrentManipulationType)
      {
        case EditorManipulationType::Translate:
          guizmoManip = ImGuizmo::TRANSLATE;
          break;
        case EditorManipulationType::Rotate:
          guizmoManip = ImGuizmo::ROTATE;
          break;
        default:
          SP_ASSERT(false, "Unknown manip type!");
          break;
      }
      ImGuizmo::Manipulate(glm::value_ptr(camera.GetViewMatrix()), glm::value_ptr(camera.GetProjectionMatrix()),
        guizmoManip, ImGuizmo::LOCAL, glm::value_ptr(transformMatrix));

      glm::vec3 translation, rotation, scale;
      ImGuizmo::DecomposeMatrixToComponents(
        glm::value_ptr(transformMatrix), glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));
      transform.Translation = translation;
      transform.Rotation = glm::radians(rotation);
    }

    ImGui::End(); // Viewport
    ImGui::PopStyleVar(3);

    // Shader lib
    ImGui::Begin("Shaders");

    const Soup::Ref<Soup::ShaderLibrary>& shaderLibrary = Soup::Renderer::GetShaderLibrary();
    for (const auto& [name, shaderData] : *shaderLibrary)
    {
      ImGui::Text("%s", name.c_str());
      ImGui::SameLine();
      ImGui::PushID(name.c_str());
      if (ImGui::Button("Reload"))
      {
        std::filesystem::path shaderFilePath = shaderData.FilePath;
        shaderData.ShaderProgram->Reload(shaderFilePath);
      }
      ImGui::PopID();
    }

    ImGui::End();

    ImGui::Begin("Renderer");

    ImGui::SeparatorText("Render statistics:");
    ImGui::Text("ts: %.3f ms", m_Dt.GetMilliseconds());
    ImGui::Text("Fps: %.3f", m_Dt.GetSeconds() == 0.0f ? 0.0f : 1.0f / m_Dt.GetSeconds());

    ImGui::SeparatorText("Renderer Settings:");

    ImGui::Columns(2);
    {
      Soup::SceneRenderer::Settings& rendererSettings = m_Renderer->GetRendererSettings();

      // Render mode
      ImGui::Text("Renderer Type");
      ImGui::NextColumn();
      Soup::RendererPreset rendererType = rendererSettings.RenderingPreset;
      std::vector<std::string> rendererTypeStrings = { "Solid", "Lighted" };
      std::string& selectedRendererTypeString = rendererTypeStrings[(int32_t)rendererType];
      if (ImGui::BeginCombo("##rendererType", selectedRendererTypeString.c_str()))
      {
        for (size_t i = 0; i < rendererTypeStrings.size(); i++)
        {
          bool isSelected = selectedRendererTypeString == rendererTypeStrings[i];
          if (ImGui::Selectable(rendererTypeStrings[i].c_str(), isSelected))
          {
            rendererSettings.RenderingPreset = (Soup::RendererPreset)i;
          }

          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
      }
      ImGui::NextColumn();

      // Selection outline width
      ImGui::Columns(1);
      ImGui::SeparatorText("Outline");
      ImGui::Columns(2);
      ImGui::Text("Line width");
      ImGui::NextColumn();
      ImGui::SliderFloat("##outlineWidthSlider", &rendererSettings.OutlineWidth, 0.01f, 5.0f);
      ImGui::NextColumn();
      ImGui::Text("Outline color");
      ImGui::NextColumn();
      ImGui::ColorEdit3("##outlineColorPicker", glm::value_ptr(rendererSettings.OutlineColor));
      ImGui::NextColumn();

      // Skybox type
      ImGui::Columns(1);
      ImGui::SeparatorText("Environment");
      ImGui::Columns(2);
      ImGui::Text("HDR image: ");
      ImGui::NextColumn();
      std::string strHdriFilePath = m_HdriFilePath.string();
      if (ImGui::InputText("##hdrImagePath", (char*)strHdriFilePath.c_str(), strHdriFilePath.capacity() + 1))
        m_HdriFilePath = strHdriFilePath;
      ImGui::NextColumn();
      ImGui::Columns(1);
      if (ImGui::Button("Import texture"))
      {
        ImportHdriFile();
      }
      ImGui::Columns(2);

      ImGui::Text("Skybox Type");
      ImGui::NextColumn();
      Soup::RendererBackground background = rendererSettings.Background;
      std::vector<std::string> rendererBackgroundStr = { "Clear color", "Environment Map", "Irradiance Map",
        "PreFiltered Map" };
      const std::string& currentRendererBackground = rendererBackgroundStr[(int32_t)background];
      if (ImGui::BeginCombo("##skyboxType", currentRendererBackground.c_str()))
      {
        for (size_t i = 0; i < rendererBackgroundStr.size(); i++)
        {
          bool isSelected = currentRendererBackground == rendererBackgroundStr[i];
          if (ImGui::Selectable(rendererBackgroundStr[i].c_str(), isSelected))
          {
            rendererSettings.Background = (Soup::RendererBackground)i;
          }

          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
      }
      ImGui::NextColumn();

      // Specular skybox mipmap level
      bool levelOptionEnabled = rendererSettings.Background == Soup::RendererBackground::PreFilteredMap ||
        rendererSettings.Background == Soup::RendererBackground::EnvironmentMap;
      if (!levelOptionEnabled)
        rendererSettings.SkyboxMipFactor = 0.0f;

      ImGui::BeginDisabled(!levelOptionEnabled);
      ImGui::Text("Skybox Mip Factor:");
      ImGui::NextColumn();
      ImGui::SliderFloat("##SkyboxMipFactor", &rendererSettings.SkyboxMipFactor, 0.0f, 1.0f);
      ImGui::NextColumn();
      ImGui::EndDisabled();
    }
    ImGui::Columns(1);

    ImGui::End(); // Renderer

    m_LegoPartsExplorerPanel->OnGuiRender();
    m_ScenePartsListPanel->OnGuiRender();
    m_LegoPartPropertiesPanel->OnGuiRender();

    EndDockspace();
  }

  void ApplicationLayer::BeginDockspace()
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
      window_flags |=
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
      window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the
    // pass-thru hole, so we ask Begin() to not render a background.
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

  void ApplicationLayer::EndDockspace()
  {
    ImGui::End();
  }

  void ApplicationLayer::OnFocusEntity(Soup::Entity entity)
  {
    Soup::TransformComponent& transform = entity.GetComponent<Soup::TransformComponent>();
    m_CameraControl->SetTargetPoint(transform.Translation);
  }

  void ApplicationLayer::ImportHdriFile()
  {
    // HDRI environment
    Soup::Texture2DSpecifications hdriTextureSpecs;
    hdriTextureSpecs.Format = Soup::TextureFormat::RGBFloat32;
    hdriTextureSpecs.WrappingModeU = Soup::TextureWrapMode::ClampToEdge;
    hdriTextureSpecs.WrappingModeV = Soup::TextureWrapMode::ClampToEdge;
    Soup::Ref<Soup::Texture2D> hdriTexture = Soup::Texture2D::Create(hdriTextureSpecs, m_HdriFilePath);

    Soup::EnvironmentCreateInfo environmentCreateInfo;

    m_Renderer->SetEnvironment(hdriTexture, environmentCreateInfo);
  }

}
