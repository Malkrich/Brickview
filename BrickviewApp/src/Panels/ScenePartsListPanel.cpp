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
		const auto& legoMeshRegistry = m_sceneContext->m_legoMeshRegistry;
		for (const auto& part : legoMeshRegistry)
		{
			LegoPartID id = part.first;
			ImGui::Text("%s", id.c_str());
		}

		ImGui::End();
	}

	void ScenePartsListPanel::setSelectedEntity(int32_t entityID)
	{
		if (entityID == -1)
		{
			m_selectedEntity = {};
			return;
		}

		m_selectedEntity = Entity((entt::entity)entityID, m_sceneContext.get());
	}

}