#pragma once

#include "GUI/Gizmo.h"
#include "GUI/Panel.h"
#include "Renderer/Texture2D.h"

#include <glm/glm.hpp>

namespace Brickview
{

  class ViewportPanel : public Soup::Panel
  {
  public:
    ViewportPanel(uint32_t width, uint32_t height);

    ///@{
    /**
     * Return the current width and height of the viewport image.
     */
    uint32_t GetWidth() const { return m_ViewportDimensions.x; }
    uint32_t GetHeight() const { return m_ViewportDimensions.y; }
    ///@}

    /**
     * Return the position, of the cursor in the viewport space.
     * If the cursor if out of the viewport panel bounds, return a { -1, -1 } vector.
     */
    glm::ivec2 GetCursorPosition() const { return m_CursorPosition; }

    /**
     * Set the color image to display in the viewport.
     */
    void SetViewportImage(Soup::Ref<Soup::Texture2D> viewportImage) { m_ViewportImage = viewportImage; }

    Soup::Gizmo& GetGizmo() { return m_Gizmo; }

  protected:
    /**
     * Handle panel GUI rendering.
     */
    void RenderPanelContent() override;

  private:
    glm::uvec2 m_ViewportDimensions;
    glm::ivec2 m_CursorPosition = { -1, -1 };

    Soup::Ref<Soup::Texture2D> m_ViewportImage = nullptr;
    Soup::Gizmo m_Gizmo;
  };

}
