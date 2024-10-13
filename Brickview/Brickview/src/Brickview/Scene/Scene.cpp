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

			glm::mat4 transform = entity.getComponent<TransformComponent>().getTransform();
			const Ref<Mesh>& legoMesh = entity.getComponent<LegoPartComponent>().Geometry;
			// TEMP: add material to ECS
			Material material = Material();

			Lego3DRenderer::drawMesh(legoMesh, material, transform);
		}

		Lego3DRenderer::end();
	}

}