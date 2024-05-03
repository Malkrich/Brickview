#pragma once

#include "Models/Mesh.h"
#include "Renderer/Light.h"

#include "Renderer/Camera.h"
#include "Renderer/Light.h"

#include <glm/glm.hpp>

namespace Brickview
{

	class SolidRenderer
	{
	public:
		static void init();
		static void shutdown();

		static void begin(const Camera& camera, const Light& light);
		static void end();
		static void flush();

		static void submitMesh(const std::shared_ptr<Mesh>& mesh, const glm::mat4& transform);
	};

}