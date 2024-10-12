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
			const auto& filePath = dirEntry.path();
			std::string itemName = filePath.stem().string();
			ImGui::Button(itemName.c_str());

			if (i > 10)
				break;

			i++;
		}

		ImGui::End();
	}

}