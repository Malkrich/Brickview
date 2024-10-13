#pragma once

#include "RenderSystem.h"
#include "Renderer/Camera.h"
#include "Renderer/GpuMesh.h"
#include "Renderer/Light.h"
#include "Renderer/Material.h"
#include "Renderer/Shader/ShaderLibrary.h"

#include <glm/glm.hpp>

namespace Brickview
{

	enum class RenderType
	{
		Solid,
		Rendered
	};

	class Lego3DRenderer
	{
	public:
		static void init();
		static void shutdown();

		// Read only
		static const Ref<ShaderLibrary>& getShaderLibrary();

		static void begin(const Camera& camera, const Light& light);
		static void end();
		static void drawMesh(const Ref<GpuMesh>& mesh, const Material& material, const glm::mat4& transform);
		static void drawLegoPart(const LegoPartComponent& legoPart, const glm::mat4& transform);

		static const RenderStatistics& getRenderStatistics();
	};

}