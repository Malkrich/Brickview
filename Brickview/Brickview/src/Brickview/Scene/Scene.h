#pragma once

#include "Core/Time.h"
#include "Renderer/SceneRenderer.h"
#include "Renderer/Texture2D.h"
#include "Renderer/Mesh.h"
#include "Lego/LegoPart.h"
#include "Lego/LegoMeshRegistry.h"

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

		void onUpdate(DeltaTime dt, Ref<SceneRenderer> renderer);

	private:
		entt::registry m_registry;
		LegoMeshRegistry m_legoMeshRegistry;

		friend class Entity;
		friend class LegoPartsSetPanel;
	};

}