#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

	struct Light
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };

		Light() = default;
		Light(const Light&) = default;
		Light(const glm::vec3& position, const glm::vec3& color)
			: Position(position)
			, Color(color)
		{}
 	};

}