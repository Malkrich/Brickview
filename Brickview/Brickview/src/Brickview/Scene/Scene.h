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
		void createLegoPartEntity(LegoPartID partID, Ref<Mesh> mesh);
		void createMeshEntity(Ref<Mesh> mesh, const glm::vec3& position, const RendererMaterial& material);
		void createLightEntity(const glm::vec3& position = glm::vec3(0.0f));
		void removeEntity(Entity e);

		void onRender(const PerspectiveCamera& camera, Ref<SceneRenderer> renderer);

	private:
		entt::registry m_registry;
		LegoPartMeshRegistry m_legoPartMeshRegistry;

		friend class Entity;
		friend class ScenePartsListPanel;
	};

}