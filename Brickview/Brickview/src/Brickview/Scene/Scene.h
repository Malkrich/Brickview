#pragma once

#include <entt.hpp>

namespace Brickview
{

	class Entity;

	class Scene
	{
	public:
		Scene() = default;

		Entity createEntity();

	private:
		entt::registry m_registry;

		friend class Entity;
	};

}