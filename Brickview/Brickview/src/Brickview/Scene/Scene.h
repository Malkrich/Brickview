#pragma once

#include "Core/Time.h"
#include "Renderer/PerspectiveCamera.h"
#include "Renderer/RendererMaterial.h"
#include "Renderer/Mesh.h"
#include "Lego/LegoPart.h"
#include "Lego/LegoPartMeshRegistry.h"

#include <entt.hpp>

namespace Brickview
{

	class Entity;
	class SceneRenderer;

	class Scene
	{
	public:
		Scene();

		Entity createEntity();
		Entity createLegoPartEntity(LegoPartID partID, Ref<Mesh> mesh);
		Entity createMeshEntity(Ref<Mesh> mesh, const glm::vec3& position, const RendererMaterial& material);
		Entity createMeshEntity(Ref<Mesh> mesh, const RendererMaterial& material);
		Entity createMeshEntity(Ref<Mesh> mesh, const glm::vec3& position);
		Entity createMeshEntity(Ref<Mesh> mesh);
		Entity createLightEntity(const glm::vec3& position, const glm::vec3& color);
		Entity createLightEntity(const glm::vec3& position = glm::vec3(0.0f));
		void removeEntity(Entity e);

		void onRender(const PerspectiveCamera& camera, Ref<SceneRenderer> renderer);

	private:
		entt::registry m_registry;
		LegoPartMeshRegistry m_legoPartMeshRegistry;

		friend class Entity;
		friend class ScenePartsListPanel;
	};

}