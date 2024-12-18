#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

	struct Material
	{
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };

		Material() = default;
		Material(const glm::vec3& color)
			: Color(color) {}
	};

}