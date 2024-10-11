#include "Pch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "Renderer/Renderer/Lego3DRenderer.h"
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

	void Scene::onUpdate(DeltaTime dt, const Camera& camera, const Light& light)
	{
		// Rendering
		Lego3DRenderer::begin(camera, light);

		//auto meshEntities = m_registry.view<TransformComponent, LegoPartComponent>();
		//for (auto e : meshEntities)
		//{
		//	Entity entity = Entity(e, this);
		//	glm::mat4 transform = entity.getComponent<TransformComponent>().getTransform();
		//	auto& meshComponent = entity.getComponent<LegoPartComponent>();
		//	Material material;
		//	Lego3DRenderer::drawMesh(meshComponent.Mesh, material, transform);
		//}

		Lego3DRenderer::end();
	}

}