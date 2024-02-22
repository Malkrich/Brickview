#pragma once

#include <glm/glm.hpp>

#include "Renderer/Buffer/Buffer.h"

namespace Brickview
{

	class RenderCommand
	{
	public:
		static void setClearColor(float r, float g, float b, float a);
		static void setClearColor(const glm::vec3& clearColor);
		static void setClearColor(const glm::vec4& clearColor);
		static void clear();

		static void draw(const std::shared_ptr<VertexArray>& vertexArray);
	};

}