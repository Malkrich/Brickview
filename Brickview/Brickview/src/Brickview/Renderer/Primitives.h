#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

	struct Line
	{
		glm::vec3 P0 = { 0.0f, 0.0f, 0.0f };
		glm::vec3 P1 = { 1.0f, 0.0f, 0.0f };

		Line() = default;
		Line(const glm::vec3& p0, const glm::vec3& p1)
			: P0(p0), P1(p1) {}
	};

}