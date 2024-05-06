#pragma once

#include "Models/Mesh.h"
#include "Renderer/Material.h"

#include <glm/glm.hpp>

namespace Brickview
{

	enum class RendererType
	{
		Solid,
		Rendered
	};

	class Renderer
	{
	public:
		static void init();
		static void shutdown();

		static void setRenderType(RendererType type);

		static void submitMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform);
	};

}