#pragma once

#include "Renderer/Camera.h"
#include "Models/Mesh.h"
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

		static void submitMesh(const Ref<Mesh>& mesh, const glm::mat4& transform);
	};

}