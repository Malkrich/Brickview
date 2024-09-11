#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

	class World
	{
	public:
		static inline constexpr glm::vec3 getXUnitVector()
		{
			return { 1.0f, 0.0f, 0.0f };
		}
		static inline constexpr glm::vec3 getYUnitVector()
		{
			return { 0.0f, 1.0f, 0.0f };
		}
		static inline constexpr glm::vec3 getZUnitVector()
		{
			return { 0.0f, 0.0f, 1.0f };
		}

		static inline constexpr glm::vec3 getUpVector()
		{
			return getYUnitVector();
		}
	};

}