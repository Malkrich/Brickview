#pragma once

#include "Camera.h"

#include "Models/Mesh.h"
#include "Material/Material.h"

namespace Brickview
{

	class LegoRenderer
	{
	public:
		static void init();
		static void shutdown();

		static void drawPiece(std::shared_ptr<Mesh> mesh, const Material& material, const glm::mat4& transform);

		static void drawLight();
	};

}