#include "Pch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

#include <glm/glm.hpp>

namespace Brickview
{

	Entity Scene::createEntity()
	{
		entt::entity enttEntity = m_registry.create();

		Entity entity = Entity(enttEntity, this);
		entity.addComponent<TransformComponent>();

		return entity;
	}

}