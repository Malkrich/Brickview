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

	Entity Scene::createLegoPartEntity(LegoPartID partID, Ref<Mesh> mesh)
	{
		Entity e = createEntity();
		// Note: LegoPartComponent should contain a LegoMaterial 
		// which yield to the right PBR Material thanks to a LegoPartMaterialRegistry class
		e.addComponent<LegoPartComponent>(partID, m_legoPartMeshRegistry, mesh);

		e.addComponent<MaterialComponent>();

		return e;
	}

	Entity Scene::createMeshEntity(Ref<Mesh> mesh, const glm::vec3& position, const RendererMaterial& material)
	{
		Entity e = createEntity();

		// Geometry
		e.addComponent<MeshComponent>(mesh);
		// Position
		TransformComponent& transform = e.getComponent<TransformComponent>();
		transform.Translation = position;
		// Material
		e.addComponent<MaterialComponent>(material);

		return e;
	}

	Entity Scene::createLightEntity(const glm::vec3& position, const glm::vec3& color)
	{
		Entity e = createEntity();

		// Position
		TransformComponent& transform = e.getComponent<TransformComponent>();
		transform.Translation = position;
		// Light
		e.addComponent<LightComponent>(color);

		return e;
	}

	Entity Scene::createLightEntity(const glm::vec3& position)
	{
		glm::vec3 color = { 1.0f, 1.0f, 1.0f };
		return createLightEntity(position, color);
	}

	Entity Scene::createMeshEntity(Ref<Mesh> mesh, const RendererMaterial& material)
	{
		glm::vec3 position = glm::vec3(0.0f);
		return createMeshEntity(mesh, position, material);
	}

	Entity Scene::createMeshEntity(Ref<Mesh> mesh, const glm::vec3& position)
	{
		RendererMaterial material;
		return createMeshEntity(mesh, position, material);
	}

	Entity Scene::createMeshEntity(Ref<Mesh> mesh)
	{
		glm::vec3 position = glm::vec3(0.0f);
		RendererMaterial material;
		return createMeshEntity(mesh, position, material);
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
			SceneLightsData scenelightsData;
			// Point lights
			auto lightEntities = m_registry.view<TransformComponent, LightComponent>();
			size_t pointLightCount = lightEntities.size_hint();
			scenelightsData.PointLights.reserve(pointLightCount);
			scenelightsData.PointLightIDs.reserve(pointLightCount);
			for (auto e : lightEntities)
			{
				Entity entity = { e, this };
				uint32_t entityID = (uint32_t)e;
				const glm::vec3& position = entity.getComponent<TransformComponent>().Translation;
				const glm::vec3& color = entity.getComponent<LightComponent>().Color;

				scenelightsData.PointLights.emplace_back(position, color);
				scenelightsData.PointLightIDs.emplace_back(entityID);
			}
			renderer->setSceneEnvironment(camera, scenelightsData);
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