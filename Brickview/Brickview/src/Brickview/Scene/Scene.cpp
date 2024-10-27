#include "Pch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "Renderer/Light.h"

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
		e.addComponent<LegoPartComponent>(partID, m_legoPartMeshRegistry, mesh);
	}

	void Scene::onUpdate(DeltaTime dt, Ref<SceneRenderer> renderer)
	{
		// Rendering
		// TODO: add lights to ECS and rendering
		auto meshEntities = m_registry.view<TransformComponent, LegoPartComponent>();
		for (auto e : meshEntities)
		{
			Entity entity = { e, this };

			const TransformComponent& transform = entity.getComponent<TransformComponent>();
			const LegoPartComponent& legoPart   = entity.getComponent<LegoPartComponent>();
			renderer->submitLegoPart(legoPart, m_legoPartMeshRegistry, transform, (uint32_t)e);
		}
	}

}