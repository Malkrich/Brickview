#include "LegoPartPropertiesPanel.h"

#include "Scene/BrickviewComponents.h"

#include "Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace Brickview
{

  namespace Utils
  {

    template <typename Component>
    static void DrawComponent(const std::string& name, Soup::Entity e, void (*drawFunction)(Component&))
    {
      if (e.HasComponent<Component>())
      {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

        ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
          ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;
        if (ImGui::TreeNodeEx((void*)typeid(Component).hash_code(), treeNodeFlags, "%s", name.c_str()))
        {
          ImGui::Columns(2);
          Component& component = e.GetComponent<Component>();
          drawFunction(component);
          ImGui::Columns(1);

          ImGui::TreePop();
        }

        ImGui::PopStyleVar();
      }
    }

  }

  LegoPartPropertiesPanel::LegoPartPropertiesPanel(Soup::Ref<BrickviewScene> scene)
    : m_Scene(scene)
  {
  }

  void LegoPartPropertiesPanel::OnGuiRender()
  {
    bool deleteEntity = false;

    ImGui::Begin("Lego Part Properties");
    if (HasEntityContext())
    {
      deleteEntity = ImGui::Button("Delete");
      Soup::Entity entity = GetEntityContext();
      DrawEntityComponents(entity);
    }
    ImGui::End();

    if (deleteEntity)
    {
      m_Scene->RemoveEntity(m_EntityContext);
      m_EntityContext = Soup::Entity();
    }
  }

  void LegoPartPropertiesPanel::DrawEntityComponents(Soup::Entity entity)
  {
    Utils::DrawComponent<LegoPartComponent>("Lego Part", entity,
      [](LegoPartComponent& legoPartComponent)
      {
        LegoPartID partID = legoPartComponent.ID;
        ImGui::Text("Lego Part ID");
        ImGui::NextColumn();
        ImGui::Text("%s", partID.c_str()); // TODO: ID should be an integer
        ImGui::NextColumn();
      });

    Utils::DrawComponent<Soup::TransformComponent>("Transform", entity,
      [](Soup::TransformComponent& transform)
      {
        ImGui::Text("Position");
        ImGui::NextColumn();
        ImGui::DragFloat3("##TransformPosition", (float*)glm::value_ptr(transform.Translation), 0.001f);
        ImGui::NextColumn();
      });

    Utils::DrawComponent<Soup::LightComponent>("Light", entity,
      [](Soup::LightComponent& lightComponent)
      {
        ImGui::Text("Light Color");
        ImGui::NextColumn();
        ImGui::ColorEdit3("##LightColor", (float*)glm::value_ptr(lightComponent.Color));
        ImGui::NextColumn();
      });

    Utils::DrawComponent<Soup::MaterialComponent>("Material", entity,
      [](Soup::MaterialComponent& materialComponent)
      {
        Soup::RendererMaterial& material = materialComponent.Material;
        ImGui::Text("Color");
        ImGui::NextColumn();
        ImGui::ColorEdit4("##albedo", (float*)glm::value_ptr(material.Albedo));
        ImGui::NextColumn();
        ImGui::Text("Roughness");
        ImGui::NextColumn();
        ImGui::SliderFloat("##rougnessSlider", &material.Roughness, 0.0f, 1.0f, "%.2f");
        ImGui::NextColumn();
        ImGui::Text("Metalness");
        ImGui::NextColumn();
        ImGui::SliderFloat("##metalnessSlider", &material.Metalness, 0.0f, 1.0f, "%.2f");
        ImGui::NextColumn();
      });
  }

}
