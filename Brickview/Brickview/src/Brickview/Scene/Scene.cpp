#include "Pch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "Renderer/Lights.h"

#include <glm/glm.hpp>

namespace Brickview
{

	Scene::Scene()
	{
	}

	Entity Scene::createEntity()
	{
		entt::entity enttEntity = m_registry.create();

		Entity entity = Entity(enttEntity, this);
		entity.addComponent<TransformComponent>();

		return entity;
	}

	void Scene::createLegoPartEntity(LegoPartID partID, Ref<Mesh> mesh)
	{
		Entity e = createEntity();
		// Note: LegoPartComponent should contain a LegoMaterial 
		// which yield to the right PBR Material thanks to a LegoPartMaterialRegistry class
		e.addComponent<LegoPartComponent>(partID, m_legoPartMeshRegistry, mesh);

		// TEMP
		e.addComponent<MaterialComponent>();
	}

	void Scene::removeEntity(Entity e)
	{
		BV_ASSERT(m_registry.valid((entt::entity)e), "Unvalid entity to remove!");
		m_registry.destroy((entt::entity)e);
	}

	void Scene::onRender(const PerspectiveCamera& camera, Ref<SceneRenderer> renderer)
	{
		// Lights
		{
			SceneEnvironment env;
			// Point lights
			auto lightEntities = m_registry.view<TransformComponent, LightComponent>();
			size_t pointLightCount = lightEntities.size_hint();
			env.PointLights.reserve(pointLightCount);
			env.PointLightIDs.reserve(pointLightCount);
			for (auto e : lightEntities)
			{
				Entity entity = { e, this };
				uint32_t entityID = (uint32_t)e;
				const glm::vec3& position = entity.getComponent<TransformComponent>().Translation;
				const glm::vec3& color = entity.getComponent<LightComponent>().Color;

				env.PointLights.emplace_back(position, color);
				env.PointLightIDs.emplace_back(entityID);
			}
			renderer->begin(camera, env);
		}

		// Lego meshes
		{
			auto legoPartEntities = m_registry.view<TransformComponent, LegoPartComponent, MaterialComponent>();
			for (auto e : legoPartEntities)
			{
				Entity entity = { e, this };

				const TransformComponent& transform = entity.getComponent<TransformComponent>();
				const LegoPartComponent& legoPart = entity.getComponent<LegoPartComponent>();
				const MaterialComponent& material = entity.getComponent<MaterialComponent>();
				renderer->submitLegoPart(legoPart, m_legoPartMeshRegistry, transform, material, (uint32_t)e);
			}
		}

		renderer->render();
	}

}