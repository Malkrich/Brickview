#pragma once

#include <glm/glm.hpp>

namespace Brickview::MathUtils
{

	glm::vec3 SphericalToCartesian(float pitch, float yaw, float distance);

	glm::mat3 computeNormalTransform(const glm::mat4& transform);

}