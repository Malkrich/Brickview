#include "LegoPartPropertiesPanel.h"

#include <imgui.h>

namespace Brickview
{

	LegoPartPropertiesPanel::LegoPartPropertiesPanel(Entity entity)
		: m_entityContext(entity)
	{}

	void LegoPartPropertiesPanel::onGuiRender()
	{
		ImGui::Begin("Lego Part Properties");

		if (m_entityContext)
			drawEntityComponents(m_entityContext);

		ImGui::End();
	}

	void LegoPartPropertiesPanel::drawEntityComponents(Entity entity)
	{
		ImGui::Columns(2);

		if (entity.hasComponent<TransformComponent>())
		{
			ImGui::Separator();
			TransformComponent& transform = entity.getComponent<TransformComponent>();
			ImGui::Text("Position");
			ImGui::NextColumn();
			ImGui::DragFloat3("##TransformPosition", (float*)glm::value_ptr(transform.Translation), 0.001f);
			ImGui::NextColumn();
		}

		if (entity.hasComponent<LegoPartComponent>())
		{
			ImGui::Separator();
			LegoPartComponent& legoPartComponent = entity.getComponent<LegoPartComponent>();
			LegoPartID partID = legoPartComponent.ID;
			LegoMaterial& legoMaterial = legoPartComponent.Material;
			ImGui::Text("Lego Part ID"); 
			ImGui::NextColumn();
			ImGui::Text("%s", partID.c_str()); // TODO: ID should be an integer
			ImGui::NextColumn();
			ImGui::Text("Color:");
			ImGui::NextColumn();
			ImGui::ColorEdit3("##LegoColor", (float*)glm::value_ptr(legoMaterial.Color));
			ImGui::NextColumn();
		}

		if (entity.hasComponent<LightComponent>())
		{
			ImGui::Separator();
			LightComponent& lightCompoenent = entity.getComponent<LightComponent>();
			ImGui::Text("Light Color");
			ImGui::NextColumn();
			ImGui::ColorEdit3("##LightColor", (float*)glm::value_ptr(lightCompoenent.Color));
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
	}

}