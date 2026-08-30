#include "ViewportPanel.h"

#include <imgui.h>

namespace Brickview
{

  ViewportPanel::ViewportPanel(uint32_t width, uint32_t height)
    : m_ViewportDimensions(width, height)
  {
    m_StyleVariables.Add(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    m_StyleVariables.Add(ImGuiStyleVar_WindowRounding, 0.0f);
    m_StyleVariables.Add(ImGuiStyleVar_WindowBorderSize, 0.0f);
  }

  void ViewportPanel::RenderPanelContent()
  {
    if (!m_ViewportImage)
      return;

    m_ViewportDimensions = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
    ImGui::Image((ImTextureID)m_ViewportImage->GetHandle(), ImVec2(m_ViewportDimensions.x, m_ViewportDimensions.y),
      ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

    glm::ivec2 windowPos = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
    glm::ivec2 cursorAbsolutePos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };
    glm::ivec2 cursorRelativePos = cursorAbsolutePos - windowPos;
    if (cursorRelativePos.x < 0 || cursorRelativePos.y < 0 || cursorRelativePos.x > m_ViewportDimensions.x ||
      cursorRelativePos.y > m_ViewportDimensions.y)
      m_CursorPosition = { -1, -1 };
    else
      m_CursorPosition = cursorRelativePos;

    m_Gizmo.SetContext(ImGui::GetCurrentWindow(), ImGui::GetForegroundDrawList());
    m_Gizmo.OnRender(windowPos, m_ViewportDimensions);

    // Focus if middle mouse button clicked because it controls the viewport
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
      ImGui::SetWindowFocus("Viewport");

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
      ImGui::OpenPopup("rightClickViewportMenu");

    if (ImGui::BeginPopup("rightClickViewportMenu"))
    {
      if (ImGui::Button("Light"))
      {
        SP_ASSERT(false, "Not implemented yet!");
        // m_Scene->CreateLightEntity();
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

}
