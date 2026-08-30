#include "ApplicationLayer.h"

#include "Panels/LegoPartPropertiesPanel.h"
#include "Panels/LegoPartsExplorerPanel.h"
#include "Panels/RendererSettingsPanel.h"
#include "Panels/ScenePartsCollectionPanel.h"
#include "Panels/ShadersPanel.h"
#include "Panels/ViewportPanel.h"

#include "Lego/LegoMeshReader.h"
#include "Scene/BrickviewScene.h"

#include "Core/Input.h"
#include "Core/KeyCodes.h"
#include "Core/MouseButtons.h"
#include "IO/MeshImporter.h"
#include "Renderer/Renderer.h"

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

  void ApplicationLayer::OnAttach()
  {
    Soup::MeshImporter::AddReader<LegoMeshReader>(".dat");

    // Scene
    m_Scene = Soup::CreateRef<BrickviewScene>();
    Soup::Ref<Soup::PerspectiveCamera> camera = Soup::CreateRef<Soup::PerspectiveCamera>();
    m_Scene->CreateCameraEntity(camera, true);
    m_CameraController.SetCamera(camera);
    m_CameraController.SetLaptopMode(m_LaptopMode);

    // Renderer
    // Note: think about the dimensions, this is the native window size
    // not the actual ImGui viewport size
    glm::ivec2 viewportDim = Soup::Input::GetWindowSize();
    m_Renderer = Soup::CreateRef<Soup::SceneRenderer>(viewportDim.x, viewportDim.y);

    // Panels
    m_PanelManager = Soup::CreateScope<Soup::PanelManager>();
    m_PanelManager->AddPanel<LegoPartsExplorerPanel>("Lego parts explorer", "./data/LDraw/parts/");
    m_PanelManager->GetPanel<LegoPartsExplorerPanel>()->SetOnLoadCallbackFunction(
      [&](const std::filesystem::path& filePath)
      {
        LegoPartID partID = filePath.stem().string();
        Soup::Ref<Soup::Mesh> mesh = Soup::MeshImporter::Import(filePath);
        m_Scene->CreateLegoPartEntity(partID, mesh);
      });
    m_PanelManager->AddPanel<LegoPartPropertiesPanel>("Lego parts properties", m_Scene);
    m_PanelManager->AddPanel<ScenePartsCollectionPanel>("Lego Parts Collection", m_Scene);
    m_PanelManager->AddPanel<ViewportPanel>("Viewport", viewportDim.x, viewportDim.y);
    m_PanelManager->GetPanel<ViewportPanel>()->GetGizmo().SetCamera(camera);
    m_PanelManager->GetPanel<ViewportPanel>()->GetGizmo().SetManipulationType(Soup::Gizmo::ManipulationType::Translate);
    m_PanelManager->AddPanel<RendererSettingsPanel>("Renderer Settings", m_Renderer);
    m_PanelManager->AddPanel<ShadersPanel>("Shaders");
  }

  void ApplicationLayer::OnDetach() {}

  void ApplicationLayer::OnEvent(Soup::Event& event)
  {
    m_CameraController.OnEvent(event);

    Soup::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Soup::KeyPressedEvent>([&](const Soup::KeyPressedEvent& event) { return OnKeyPressed(event); });
    dispatcher.Dispatch<Soup::MousePressedEvent>(
      [&](const Soup::MousePressedEvent& event) { return OnMousePressed(event); });
  }

  bool ApplicationLayer::OnKeyPressed(const Soup::KeyPressedEvent& event)
  {
    Soup::Ref<ViewportPanel> viewportPanel = m_PanelManager->GetPanel<ViewportPanel>();
    switch (event.GetKeyCode())
    {
      case Soup::KeyCode::Escape:
        viewportPanel->GetGizmo().SetManipulationType(Soup::Gizmo::ManipulationType::None);
        break;
      case Soup::KeyCode::T:
        viewportPanel->GetGizmo().SetManipulationType(Soup::Gizmo::ManipulationType::Translate);
        break;
      case Soup::KeyCode::R:
        viewportPanel->GetGizmo().SetManipulationType(Soup::Gizmo::ManipulationType::Rotate);
        break;
      case Soup::KeyCode::F:
      {
        Soup::Entity selectedEntity = m_PanelManager->GetPanel<LegoPartPropertiesPanel>()->GetEntityContext();
        if (selectedEntity)
          OnFocusEntity(selectedEntity);
        break;
      }
      default:
        break;
    }

    return true;
  }

  bool ApplicationLayer::OnMousePressed(const Soup::MousePressedEvent& event)
  {
    Soup::Ref<ViewportPanel> viewportPanel = m_PanelManager->GetPanel<ViewportPanel>();
    if (event.GetMouseButton() == Soup::MouseButton::Left && !viewportPanel->GetGizmo().IsHovered())
    {
      glm::ivec2 cursorPosition = viewportPanel->GetCursorPosition();
      if (cursorPosition == glm::ivec2(-1, -1))
        return false;

      glm::ivec2 pixelPosition = { cursorPosition.x, viewportPanel->GetHeight() - cursorPosition.y };
      int32_t entityID = m_Renderer->GetEntityIDImage()->GetPixelInteger(pixelPosition.x, pixelPosition.y);

      Soup::Entity selectedEntity = entityID == -1 ? Soup::Entity() : m_Scene->GetEntity(entityID);

      m_PanelManager->GetPanel<LegoPartPropertiesPanel>()->SetEntityContext(selectedEntity);
      viewportPanel->GetGizmo().SetManipulatedEntity(selectedEntity);
      m_Renderer->SetSelectedEntity(selectedEntity);

      return true;
    }
    return false;
  }

  void ApplicationLayer::OnUpdate(Soup::DeltaTime dt)
  {
    m_Dt = dt;

    Soup::Ref<RendererSettingsPanel> rendererSettingsPanel = m_PanelManager->GetPanel<RendererSettingsPanel>();
    rendererSettingsPanel->SetFrameDelta(dt);

    Soup::Ref<ViewportPanel> viewportPanel = m_PanelManager->GetPanel<ViewportPanel>();

    m_CameraController.Resize(viewportPanel->GetWidth(), viewportPanel->GetHeight());
    m_Renderer->ResizeViewport(viewportPanel->GetWidth(), viewportPanel->GetHeight());

    m_CameraController.SetInteractionPossible(viewportPanel->IsHovered() && !IsOnResizeRegion());
    if (!viewportPanel->GetGizmo().IsHovered())
      m_CameraController.OnUpdate();

    m_Renderer->Render(m_Scene);
    Soup::Ref<Soup::Texture2D> renderedImage = m_Renderer->GetRenderedImage();
    viewportPanel->SetViewportImage(renderedImage);
  }

  void ApplicationLayer::OnGuiRender()
  {
    BeginDockspace();

    m_PanelManager->OnGUIRender();

    // Menu
    if (ImGui::BeginMenuBar())
    {
      if (ImGui::BeginMenu("Mode"))
      {
        if (ImGui::MenuItem("Laptop mode", nullptr, m_LaptopMode))
        {
          m_LaptopMode = !m_LaptopMode;
          m_CameraController.SetLaptopMode(m_LaptopMode);
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }

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
  bool ApplicationLayer::IsOnResizeRegion() const
  {
    ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
    return cursor == ImGuiMouseCursor_ResizeNS || cursor == ImGuiMouseCursor_ResizeEW ||
      cursor == ImGuiMouseCursor_ResizeNESW || cursor == ImGuiMouseCursor_ResizeNWSE;
  }

  void ApplicationLayer::OnFocusEntity(Soup::Entity entity)
  {
    Soup::TransformComponent& transform = entity.GetComponent<Soup::TransformComponent>();
    m_CameraController.SetTargetPoint(transform.Translation);
  }

}
