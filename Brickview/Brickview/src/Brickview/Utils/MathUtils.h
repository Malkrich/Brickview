#pragma once

#include <glm/glm.hpp>

namespace Brickview::MathUtils
{

	glm::vec3 SphericalToCartesian(float pitch, float yaw, float distance);

}