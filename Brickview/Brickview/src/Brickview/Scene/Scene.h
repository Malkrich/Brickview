#pragma once

#include "Core/Time.h"
#include "Renderer/Mesh.h"
#include "Lego/LegoPart.h"
#include "Lego/LegoMeshRegistry.h"
#include "Renderer/SceneRenderer.h"

#include <entt.hpp>

namespace Brickview
{

	class Entity;

	class Scene
	{
	public:
		Scene() = default;

		Entity createEntity();
		void createLegoPartEntity(LegoPartID partID, Ref<Mesh> mesh);

		void onUpdate(DeltaTime dt, const Camera& camera);

	private:
		entt::registry m_registry;
		LegoMeshRegistry m_legoMeshRegistry;

		SceneRenderer m_renderer;

		friend class Entity;
		friend class LegoPartsCollectionPanel;
	};

}