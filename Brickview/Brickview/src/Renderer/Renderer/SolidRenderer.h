#pragma once

#include "Models/Mesh.h"
#include "Renderer/Light.h"

namespace Brickview
{

	class SolidRenderer
	{
	public:
		static void init();

		static void submitMesh(const Mesh& mesh);
		static void submitLight(const Light& light);
	};

}