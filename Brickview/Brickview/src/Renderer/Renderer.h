#pragma once

#include "Buffer/Buffer.h"
#include "Shader/Shader.h"
#include "Camera.h"

namespace Brickview
{

	class Renderer
	{
	public:
		static void init();
		static void shutdown();

		static void onWindowResize(unsigned int width, unsigned int height);
		static void onWindowResize(const glm::ivec2& windowDimension);

		static void begin(const Camera& camera);
		static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
	};

}