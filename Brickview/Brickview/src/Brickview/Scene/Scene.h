#pragma once

#include "Core/Time.h"
#include "Renderer/SceneRenderer.h"
#include "Renderer/Texture2D.h"
#include "Renderer/Mesh.h"
#include "Lego/LegoPart.h"
#include "Lego/LegoPartMeshRegistry.h"

#include <entt.hpp>

namespace Brickview
{

	class Entity;

	class Scene
	{
	public:
		Scene();

		Entity createEntity();
		void createLegoPartEntity(LegoPartID partID, Ref<Mesh> mesh);
		void removeEntity(Entity e);

		void onRender(const PerspectiveCamera& camera, Ref<SceneRenderer> renderer);

	private:
		entt::registry m_registry;
		LegoPartMeshRegistry m_legoPartMeshRegistry;

		friend class Entity;
		friend class ScenePartsListPanel;
	};

}