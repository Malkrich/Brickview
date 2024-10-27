#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

	struct LegoMaterial
	{
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };

		LegoMaterial() = default;
		LegoMaterial(const glm::vec3& color)
			: Color(color) {}
	};

}