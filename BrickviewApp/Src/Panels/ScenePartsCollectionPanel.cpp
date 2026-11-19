#include "ScenePartsCollectionPanel.h"

#include <imgui.h>

namespace Brickview
{

  ScenePartsCollectionPanel::ScenePartsCollectionPanel(Soup::Ref<BrickviewScene> context)
    : m_SceneContext(context)
  {
  }

  void ScenePartsCollectionPanel::RenderPanelContent()
  {
    ImGui::SeparatorText("Lego Collection");
    for (const auto& part : m_SceneContext->GetLegoPartRegistry())
    {
      LegoPartID id = part.first;
      ImGui::Text("%s", id.c_str());
    }
  }

}
