#include "Pch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "Renderer/SceneRenderer.h"
#include "Renderer/Lights.h"

#include <glm/glm.hpp>

namespace Brickview
{

	Scene::Scene()
	{
	}

	Entity Scene::createEntity()
	{
		entt::entity e = m_registry.create();

		Entity entity = Entity(e, this);
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

	void Scene::createMeshEntity(Ref<Mesh> mesh, const glm::vec3& position, const RendererMaterial& material)
	{
		Entity e = createEntity();

		// Geometry
		e.addComponent<MeshComponent>(mesh);
		// Position
		TransformComponent& transform = e.getComponent<TransformComponent>();
		transform.Translation = position;
		// Material
		e.addComponent<MaterialComponent>(material);
	}

	void Scene::createLightEntity(const glm::vec3& position)
	{
		Entity e = createEntity();

		// Position
		TransformComponent& transform = e.getComponent<TransformComponent>();
		transform.Translation = position;
		// Light
		e.addComponent<LightComponent>();
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

		// Basic Meshes
		{
			auto meshEntities = m_registry.view<MeshComponent, TransformComponent, MaterialComponent>();
			for (auto e : meshEntities)
			{
				Entity entity = { e, this };
				uint32_t entityID = (uint32_t)e;

				const MeshComponent& mesh = entity.getComponent<MeshComponent>();
				const TransformComponent& transform = entity.getComponent<TransformComponent>();
				const MaterialComponent& material = entity.getComponent<MaterialComponent>();
				renderer->submitMesh(mesh, transform, material, entityID);
			}
		}

		// Lego meshes
		{
			auto legoPartEntities = m_registry.view<LegoPartComponent, TransformComponent, MaterialComponent>();
			for (auto e : legoPartEntities)
			{
				Entity entity = { e, this };

				const LegoPartComponent& legoPart = entity.getComponent<LegoPartComponent>();
				const TransformComponent& transform = entity.getComponent<TransformComponent>();
				const MaterialComponent& material = entity.getComponent<MaterialComponent>();
				renderer->submitLegoPart(legoPart, m_legoPartMeshRegistry, transform, material, (uint32_t)e);
			}
		}

		renderer->render();
	}

}