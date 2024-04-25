#pragma once

#include "Models/Mesh.h"

#include "Renderer/Camera.h"
#include "Renderer/Light.h"
#include "Material/Material.h"

namespace Brickview
{

	class RenderedRenderer
	{
	public:
		static void init();
		static void shutdown();

		static void begin(const Camera& camera, const Light& light);

		static void drawLights(bool drawLights);
		static void submitMesh(const std::shared_ptr<Mesh>& mesh, const Material& material, const glm::mat4& transform);

		static void end();

	private:
		static void submitLight();
	};

}