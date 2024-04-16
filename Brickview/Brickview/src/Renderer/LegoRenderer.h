#pragma once

#include "Buffer/Buffer.h"
#include "Shader/Shader.h"
#include "Models/Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "Material/Material.h"

namespace Brickview
{

	class LegoRenderer
	{
	public:
		static void init();
		static void shutdown();

		static void onWindowResize(unsigned int width, unsigned int height);
		static void onWindowResize(const glm::ivec2& windowDimension);

		static void drawPiece(std::shared_ptr<Mesh> mesh, const Material& material, const glm::mat4& transform);
		static void drawLight();

		static void begin(const Camera& camera, const Light& light);
		static void end();
		static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform);
	};

}