#include "ScenePartsListPanel.h"

#include <imgui.h>

namespace Brickview
{

	ScenePartsListPanel::ScenePartsListPanel(Ref<Scene> context)
		: m_sceneContext(context)
	{}

	void ScenePartsListPanel::onGuiRender()
	{
		ImGui::Begin("Lego Parts Collection");

		ImGui::SeparatorText("Lego Collection");
		for (const auto& part : m_sceneContext->m_legoPartMeshRegistry)
		{
			LegoPartID id = part.first;
			ImGui::Text("%s", id.c_str());
		}

		ImGui::End();
	}

}