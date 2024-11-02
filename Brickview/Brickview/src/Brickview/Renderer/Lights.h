#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

	struct PointLight
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };

		PointLight() = default;
		PointLight(const PointLight&) = default;
		PointLight(const glm::vec3& position, const glm::vec3& color)
			: Position(position)
			, Color(color)
		{}
 	};

}