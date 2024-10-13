#pragma once

#include "Renderer/Camera.h"
#include "Renderer/Light.h"
#include "Renderer/Material.h"
#include "Renderer/GpuMesh.h"
#include "Scene/Components.h"

#include <glm/glm.hpp>

namespace Brickview
{

	struct RenderStatistics
	{
		uint32_t DrawCalls = 0;
		uint32_t MaxInstanceCount = 0;

		RenderStatistics() = default;
	};

	class RenderSystem
	{
	public:
		virtual ~RenderSystem() = default;

		virtual void begin(const Camera& camera, const Light& light) = 0;
		virtual void end() = 0;

		virtual const RenderStatistics& getRenderStatistics() const { return RenderStatistics(); }

		virtual void drawMesh(const Ref<GpuMesh>& mesh, const Material& material, const glm::mat4& transform) {}
		virtual void drawLegoPart(const LegoPartComponent& legoPart, const glm::mat4& transform) {}
		virtual void drawLights(const Light& light) {}
	};

}