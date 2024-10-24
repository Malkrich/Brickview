#include "LegoPartsSetPanel.h"

#include <imgui.h>

namespace Brickview
{

	LegoPartsSetPanel::LegoPartsSetPanel(Ref<Scene> context)
		: m_sceneContext(context)
	{}

	void LegoPartsSetPanel::onGuiRender()
	{
		ImGui::Begin("Lego Parts Collection");

		ImGui::SeparatorText("Lego Collection");
		const auto& legoMeshRegistry = m_sceneContext->m_legoMeshRegistry;
		for (const auto& part : legoMeshRegistry)
		{
			LegoPartID id = part.first;
			ImGui::Text("%s", id.c_str());
		}

		// TEMP
		ImGui::SeparatorText("Entities");

		auto legoInstances = m_sceneContext->m_registry.view<TransformComponent, LegoPartComponent>();
		for (auto e : legoInstances)
		{
			Entity entity = { e, m_sceneContext.get() };
			LegoPartID partID = entity.getComponent<LegoPartComponent>().ID;
			glm::vec3& translation = entity.getComponent<TransformComponent>().Translation;

			std::string posName = std::format("{}##{}", partID, (uint32_t)entity);
			ImGui::DragFloat3(posName.c_str(), glm::value_ptr(translation), 0.005f);
		}

		ImGui::End();
	}

	void LegoPartsSetPanel::setSelectedEntity(int32_t entityID)
	{
		if (entityID == -1)
		{
			m_selectedEntity = {};
			return;
		}

		m_selectedEntity = Entity((entt::entity)entityID, m_sceneContext.get());
	}

}