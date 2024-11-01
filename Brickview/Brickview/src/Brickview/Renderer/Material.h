#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

	struct RendererMaterial
	{
		glm::vec4 Albedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Roughness = 1.0;
		float Metalness = 0.0;

		RendererMaterial() = default;
	};

}