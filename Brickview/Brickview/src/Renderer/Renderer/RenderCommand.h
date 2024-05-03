#pragma once

#include "Renderer/Buffer/Buffer.h"

#include <glm/glm.hpp>

namespace Brickview
{

	class RenderCommand
	{
	public:
		static void init();

		static void setClearColor(float r, float g, float b, float a);
		static void setClearColor(float r, float g, float b);
		static void setClearColor(const glm::vec3& clearColor);
		static void setClearColor(const glm::vec4& clearColor);
		static void clear();

		static void setViewportDimension(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

		static void draw(const std::shared_ptr<VertexArray>& vertexArray);
	};

}