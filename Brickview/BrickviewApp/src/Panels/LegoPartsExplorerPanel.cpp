#include "LegoPartsExplorerPanel.h"

#include <imgui.h>

namespace Brickview
{

	LegoPartsExplorerPanel::LegoPartsExplorerPanel(const std::filesystem::path& partsDirectory)
		: m_legoPartsDirectory(partsDirectory)
	{
		m_defaultLegoPartIcon = createRef<Texture2D>("./data/Icons/legoPartIcon.png");
	}

	void LegoPartsExplorerPanel::onImGuiRender()
	{
		ImGui::Begin("Lego Part Explorer");

		//if (ImGui::BeginPopupContextWindow(0))
		//{
		//	if (ImGui::MenuItem("Load file"))
		//		m_onFileLoadCallback(m_legoPartsDirectory / "1.dat");

		//	ImGui::EndPopup();
		//}

		uint32_t i = 0;
		for (const auto& dirEntry : std::filesystem::directory_iterator(m_legoPartsDirectory))
		{
			if (i < m_fileOffset)
			{
				i++;
				continue;
			}

			const auto& filePath = dirEntry.path();
			std::string itemName = filePath.stem().string();
			if (ImGui::Button(itemName.c_str()))
			{
				Ref<Mesh> mesh = Mesh::load(filePath);
				m_onFileLoadCallback(mesh);
			}

			if (i > m_fileOffset + m_maxDisplayedFile)
				break;

			i++;
		}

		ImGui::End();
	}

}