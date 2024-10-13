#include "Pch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "Renderer/Renderer/Lego3DRenderer.h"
#include "Renderer/Light.h"
#include "Renderer/Material.h"

#include <glm/glm.hpp>

namespace Brickview
{

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
		e.addComponent<LegoPartComponent>(partID, &m_legoMeshRegistry, mesh);
	}

	void Scene::onUpdate(DeltaTime dt, const Camera& camera)
	{
		// Rendering
		// TEMP: add light to ECS
		Light light;
		Lego3DRenderer::begin(camera, light);

		auto meshEntities = m_registry.view<TransformComponent, LegoPartComponent>();
		for (auto e : meshEntities)
		{
			Entity entity = { e, this };

			auto transform       = entity.getComponent<TransformComponent>().getTransform();
			const auto& legoPart = entity.getComponent<LegoPartComponent>();
			Lego3DRenderer::drawLegoPart(legoPart, transform);
		}

		Lego3DRenderer::end();
	}

}