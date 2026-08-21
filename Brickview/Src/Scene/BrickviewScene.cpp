#include "BrickviewScene.h"

#include "BrickviewComponents.h"

#include "Scene/Entity.h"

namespace Brickview
{

  Soup::Entity BrickviewScene::CreateLegoPartEntity(LegoPartID partID, Soup::Ref<Soup::Mesh> mesh)
  {
    Soup::Entity entity = CreateMeshEntity(mesh);

    m_LegoPartMeshRegistry.AddPart(partID, mesh);
    entity.AddComponent<LegoPartComponent>(partID);

    return entity;
  }

#if 0
  void Scene::SubmitToRenderer(const PerspectiveCamera& camera, Ref<SceneRenderer> renderer)
  {
    // Lights
    {
      SceneLightsData scenelightsData;
      // Point lights
      auto lightEntities = m_Registry.view<TransformComponent, LightComponent>();
      size_t pointLightCount = lightEntities.size_hint();
      scenelightsData.PointLights.reserve(pointLightCount);
      scenelightsData.PointLightIDs.reserve(pointLightCount);
      for (auto e : lightEntities)
      {
        Entity entity = { e, this };
        uint32_t entityID = (uint32_t)e;
        const glm::vec3& position = entity.GetComponent<TransformComponent>().Translation;
        const glm::vec3& color = entity.GetComponent<LightComponent>().Color;

        scenelightsData.PointLights.emplace_back(position, color);
        scenelightsData.PointLightIDs.emplace_back(entityID);
      }
      renderer->SetSceneEnvironment(camera, scenelightsData);
    }

    // Basic Meshes
    {
      auto meshEntities = m_Registry.view<MeshComponent, TransformComponent, MaterialComponent>();
      for (auto e : meshEntities)
      {
        Entity entity = { e, this };
        uint32_t entityID = (uint32_t)e;

        const MeshComponent& mesh = entity.GetComponent<MeshComponent>();
        const TransformComponent& transform = entity.GetComponent<TransformComponent>();
        const MaterialComponent& material = entity.GetComponent<MaterialComponent>();

        renderer->SubmitMesh(mesh, transform, material, entityID);
      }
    }

    // Lego meshes
    {
      auto legoPartEntities = m_Registry.view<LegoPartComponent, TransformComponent, MaterialComponent>();
      for (auto e : legoPartEntities)
      {
        Entity entity = { e, this };
        uint32_t entityId = (uint32_t)e;

        const LegoPartComponent& legoPart = entity.GetComponent<LegoPartComponent>();
        const TransformComponent& transform = entity.GetComponent<TransformComponent>();
        const MaterialComponent& material = entity.GetComponent<MaterialComponent>();

        Ref<GpuMesh> mesh = m_LegoPartMeshRegistry.GetPart(legoPart.ID);
        renderer->SubmitLegoPart(legoPart.ID, mesh, transform, material, entityId);
      }
    }
  }
#endif

}
