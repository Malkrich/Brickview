#include "LegoPartsExplorerPanel.h"

#include <imgui.h>

namespace Brickview
{

	LegoPartsExplorerPanel::LegoPartsExplorerPanel(const std::filesystem::path& partsDirectory)
		: m_legoPartsDirectory(partsDirectory)
	{
		m_defaultLegoPartIcon = Texture2D::create("./data/Icons/legoPartIcon.png");
	}

	void LegoPartsExplorerPanel::onGuiRender()
	{
		ImGui::Begin("Lego Part Explorer");

		float windowWidth = ImGui::GetContentRegionAvail().x;
		uint32_t columnCount = (uint32_t)(windowWidth / (m_iconWidth + m_iconPadding));
		ImGui::Columns(columnCount, 0, false);

		uint32_t fileIndex = 0;
		for (const auto& dirEntry : std::filesystem::directory_iterator(m_legoPartsDirectory))
		{
			if (dirEntry.is_directory())
				continue;

			if (fileIndex < m_fileOffset)
			{
				fileIndex++;
				continue;
			}

			const auto& filePath = dirEntry.path();
			std::string itemName = filePath.stem().string();

			ImGui::PushID(itemName.c_str());

			auto textureID = m_defaultLegoPartIcon->getTextureID();
			if (ImGui::ImageButton((ImTextureID)textureID, { (float)m_iconWidth, (float)m_iconWidth }))
			{
				m_onFileLoadCallback(filePath);
			}
			ImGui::TextWrapped(itemName.c_str());
			ImGui::PopID();
			ImGui::NextColumn();

			if (fileIndex > m_fileOffset + m_maxDisplayedFile)
				break;

			fileIndex++;
		}

		ImGui::End();
	}

}