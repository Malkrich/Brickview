#pragma once

#include "Panels/LegoPartPropertiesPanel.h"
#include "Panels/LegoPartsExplorerPanel.h"
#include "Panels/ScenePartsListPanel.h"

#include "Core/Event/KeyEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Layer/Layer.h"
#include "Renderer/CameraController.h"
#include "Renderer/SceneRenderer.h"
#include "Scene/BrickviewScene.h"

#include <imgui.h>

namespace Brickview
{

  class ApplicationLayer : public Soup::Layer
  {
  public:
    enum class EditorManipulationType
    {
      None = 0,
      Translate,
      Rotate
    };

  public:
    ApplicationLayer();
    ~ApplicationLayer() = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Soup::Event& e) override;
    void OnUpdate(Soup::DeltaTime dt) override;
    void OnGuiRender() override;

  private:
    // Events
    bool OnMousePressed(const Soup::MousePressedEvent& e);
    bool OnKeyPressed(const Soup::KeyPressedEvent& e);

    // Gui
    void BeginDockspace();
    void EndDockspace();

    void OnFocusEntity(Soup::Entity entity);

    void ImportHdriFile();

  private:
    // Scene
    Soup::Ref<BrickviewScene> m_Scene = nullptr;

    // Render
    Soup::Ref<Soup::SceneRenderer> m_Renderer = nullptr;
    uint32_t m_ViewportWidth, m_ViewportHeight;
    std::filesystem::path m_HdriFilePath;

    Soup::Scope<Soup::CameraController> m_CameraControl = nullptr;
    bool m_LaptopMode = true;
    EditorManipulationType m_CurrentManipulationType = EditorManipulationType::Translate;
    bool m_GizmoVisible = false;

    // Mouse
    bool m_ViewportHovered = false;
    ImVec2 m_MousePosition;
    ImVec2 m_ViewportMinBound;
    ImVec2 m_ViewportMaxBound;

    Soup::DeltaTime m_Dt = 0.0f;

    // Panels
    Soup::Scope<LegoPartsExplorerPanel> m_LegoPartsExplorerPanel = nullptr;
    Soup::Scope<ScenePartsListPanel> m_ScenePartsListPanel = nullptr;
    Soup::Scope<LegoPartPropertiesPanel> m_LegoPartPropertiesPanel = nullptr;
  };

}
