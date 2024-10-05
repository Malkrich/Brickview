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

		static void setViewportDimension(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		static void draw(const Ref<VertexArray>& vertexArray);
		static void drawInstances(const Ref<VertexArray>& vertexArray, uint32_t instanceCount);
	};

}