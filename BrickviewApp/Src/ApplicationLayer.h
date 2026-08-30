#pragma once

#include "Scene/BrickviewScene.h"

#include "Core/Event/KeyEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Layer/Layer.h"
#include "GUI/Gizmo.h"
#include "GUI/PanelManager.h"
#include "Renderer/SceneRenderer.h"
#include "Scene/CameraController.h"

#include <imgui.h>

namespace Brickview
{

  class ApplicationLayer : public Soup::Layer
  {
  public:
    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Soup::Event& e) override;
    void OnUpdate(Soup::DeltaTime dt) override;
    void OnGuiRender() override;

  private:
    // Events
    bool OnMousePressed(const Soup::MousePressedEvent& event);
    bool OnKeyPressed(const Soup::KeyPressedEvent& event);

    // Gui
    void BeginDockspace();
    void EndDockspace();

    /**
     * Return true if the cursor is on a resize region.
     * @note Current implementation just checks if the mouse cursor is one of the resize icon. This may need to be
     * checked more precisely.
     */
    bool IsOnResizeRegion() const;

    void OnFocusEntity(Soup::Entity entity);

    void ImportHdriFile();

  private:
    Soup::Ref<BrickviewScene> m_Scene = nullptr;
    Soup::CameraController m_CameraController;

    Soup::Scope<Soup::PanelManager> m_PanelManager = nullptr;

    Soup::Ref<Soup::SceneRenderer> m_Renderer = nullptr;
    std::filesystem::path m_HdriFilePath;

    bool m_LaptopMode = true;

    Soup::DeltaTime m_Dt = 0.0f;
  };

}
