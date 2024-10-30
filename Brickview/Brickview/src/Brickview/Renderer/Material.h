#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

	struct RendererMaterial
	{
		glm::vec4 Albedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		//float Roughness;
		//float Metalicness;

		RendererMaterial() = default;
	};

}