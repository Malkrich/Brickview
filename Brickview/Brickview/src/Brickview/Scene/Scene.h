#pragma once

#include "Core/Time.h"
#include "Renderer/Camera.h"

#include <entt.hpp>

namespace Brickview
{

	class Entity;

	class Scene
	{
	public:
		Scene() = default;

		Entity createEntity();

		void onUpdate(DeltaTime dt, const Camera& camera);

	private:
		entt::registry m_registry;

		friend class Entity;
	};

}