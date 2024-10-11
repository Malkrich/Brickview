#include "Pch.h"
#include "Entity.h"

namespace Brickview
{

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_entityHandle(handle)
		, m_scene(scene)
	{}

}