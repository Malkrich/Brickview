#include "ScenePartsListPanel.h"

#include <imgui.h>

namespace Brickview
{

  ScenePartsListPanel::ScenePartsListPanel(Soup::Ref<BrickviewScene> context)
    : m_SceneContext(context)
  {
  }

  void ScenePartsListPanel::OnGuiRender()
  {
    ImGui::Begin("Lego Parts Collection");

    ImGui::SeparatorText("Lego Collection");
    for (const auto& part : m_SceneContext->GetLegoPartRegistry())
    {
      LegoPartID id = part.first;
      ImGui::Text("%s", id.c_str());
    }

    ImGui::End();
  }

}
