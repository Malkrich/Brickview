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

	template<typename Component>
	static void drawComponent(const std::string& name, Entity e, void(*drawFunction)(Component&))
	{
		if (e.hasComponent<Component>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));

			ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
											| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
											| ImGuiTreeNodeFlags_AllowItemOverlap| ImGuiTreeNodeFlags_FramePadding;
			if (ImGui::TreeNodeEx((void*)typeid(Component).hash_code(), treeNodeFlags, name.c_str()))
			{
				ImGui::Columns(2);
				Component& component = e.getComponent<Component>();
				drawFunction(component);
				ImGui::Columns(1);

				ImGui::TreePop();
			}

			ImGui::PopStyleVar();
		}
	}

	void LegoPartPropertiesPanel::drawEntityComponents(Entity entity)
	{
		drawComponent<TransformComponent>("Transform", entity, [](TransformComponent& transform)
		{
			ImGui::Text("Position");
			ImGui::NextColumn();
			ImGui::DragFloat3("##TransformPosition", (float*)glm::value_ptr(transform.Translation), 0.001f);
			ImGui::NextColumn();
		});

		drawComponent<LegoPartComponent>("Lego Part", entity, [](LegoPartComponent& legoPartComponent)
		{
			LegoPartID partID = legoPartComponent.ID;
			LegoMaterial& legoMaterial = legoPartComponent.Material;
			ImGui::Text("Lego Part ID");
			ImGui::NextColumn();
			ImGui::Text("%s", partID.c_str()); // TODO: ID should be an integer
			ImGui::NextColumn();
			ImGui::Text("Color:");
			ImGui::NextColumn();
			ImGui::ColorEdit4("##LegoColor", (float*)glm::value_ptr(legoMaterial.Color));
			ImGui::NextColumn();
		});

		drawComponent<LightComponent>("Light", entity, [](LightComponent& lightComponent)
		{
			ImGui::Text("Light Color");
			ImGui::NextColumn();
			ImGui::ColorEdit3("##LightColor", (float*)glm::value_ptr(lightComponent.Color));
			ImGui::NextColumn();
		});
	}

}