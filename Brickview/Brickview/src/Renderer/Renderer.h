#pragma once

#include "Buffer/Buffer.h"
#include "Shader/Shader.h"

namespace Brickview
{

	class Renderer
	{
	public:
		static void init();
		static void shutdown();

		static void onWindowResize(unsigned int width, unsigned int height);
		static void onWindowResize(const glm::ivec2& windowDimension);

		static void begin();
		static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);
	};

}