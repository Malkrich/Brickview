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

		static const glm::vec3& getLightPosition();

		static void onWindowResize(unsigned int width, unsigned int height);
		static void onWindowResize(const glm::ivec2& windowDimension);

		static void begin(const Camera& camera, const glm::vec3& lightPosition);
		static void end();
		static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
	};

}