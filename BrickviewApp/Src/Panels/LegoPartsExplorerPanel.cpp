#include "LegoPartsExplorerPanel.h"

#include "Renderer/TextureSpecifications.h"

#include <imgui.h>

namespace Brickview
{

  LegoPartsExplorerPanel::LegoPartsExplorerPanel(const std::filesystem::path& partsDirectory)
    : m_LegoPartsDirectory(partsDirectory)
  {
    Soup::Texture2DSpecifications legoPartIconSpecs;
    legoPartIconSpecs.Format = Soup::TextureFormat::RGBA;
    m_DefaultLegoPartIcon = Soup::Texture2D::Create(legoPartIconSpecs, "./data/Icons/legoPartIcon.png");
  }

  void LegoPartsExplorerPanel::OnGuiRender()
  {
    ImGui::Begin("Lego Part Explorer");

    if (!std::filesystem::exists(m_LegoPartsDirectory))
    {
      std::filesystem::path ldrawDatabasePath = std::filesystem::absolute(m_LegoPartsDirectory);
      ImGui::Text("LDraw data base at path %s not found!", ldrawDatabasePath.generic_string().c_str());
      ImGui::End();
      return;
    }

    if (ImGui::Button("<- 50"))
    {
      m_FileOffset = m_FileOffset - 50 < 0 ? m_FileOffset - 50 : 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("50 ->"))
    {
      m_FileOffset += 50;
    }

    float windowWidth = ImGui::GetContentRegionAvail().x;
    uint32_t columnCount = (uint32_t)(windowWidth / (m_IconWidth + m_IconPadding));
    columnCount = std::max<int>(1, columnCount);
    ImGui::Columns(columnCount, 0, false);

    uint32_t fileIndex = 0;
    for (const auto& dirEntry : std::filesystem::directory_iterator(m_LegoPartsDirectory))
    {
      if (dirEntry.is_directory())
        continue;

      if (fileIndex < m_FileOffset)
      {
        fileIndex++;
        continue;
      }

      const auto& filePath = dirEntry.path();
      std::string itemName = filePath.stem().string();

      ImGui::PushID(itemName.c_str());

      uint32_t textureId = m_DefaultLegoPartIcon->GetHandle();
      std::string textureIdStr = std::to_string(textureId);
      if (ImGui::ImageButton(textureIdStr.c_str(), { textureId }, { (float)m_IconWidth, (float)m_IconWidth }))
      {
        m_OnFileLoadCallback(filePath);
      }
      ImGui::TextWrapped("%s", itemName.c_str());
      ImGui::PopID();
      ImGui::NextColumn();

      if (fileIndex > m_FileOffset + m_MaxDisplayedFile)
        break;

      fileIndex++;
    }

    ImGui::End();
  }

}
