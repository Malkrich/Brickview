#include "Pch.h"
#include "MathUtils.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

namespace Brickview::MathUtils
{
	static constexpr float s_pi = glm::pi<float>();

	glm::vec3 SphericalToCartesian(float pitch, float yaw, float distance)
	{
		float radPitch = glm::radians(pitch);
		float radYaw = glm::radians(yaw);

		glm::vec3 position =
		{
			glm::cos(-radPitch) * glm::cos(s_pi / 2.0f - radYaw),
			glm::sin(-radPitch),
			glm::cos(-radPitch) * glm::sin(s_pi / 2.0f - radYaw)
		};
		position *= distance;
		return position;
	}

}