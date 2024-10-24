#pragma once

#include "Scene.h"

#include <entt.hpp>

namespace Brickview
{

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			BV_ASSERT(!hasComponent<T>(), "Component already exists!");
			return m_scene->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& getComponent()
		{
			BV_ASSERT(hasComponent<T>(), "Component does not exist!");
			return m_scene->m_registry.get<T>(m_entityHandle);
		}

		template<typename T>
		bool hasComponent() const
		{
			return m_scene->m_registry.all_of<T>(m_entityHandle);
		}

		operator uint32_t() const { return (uint32_t)m_entityHandle; }
		operator bool() const { return m_entityHandle != entt::null && m_scene != nullptr; }

	private:
		entt::entity m_entityHandle = entt::null;
		Scene* m_scene = nullptr;
	};

}