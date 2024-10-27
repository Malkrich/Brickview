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

		if (entity.hasComponent<LegoPartComponent>())
		{
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
		}

		ImGui::Columns(1);
	}

}