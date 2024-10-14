#pragma once

#include "Renderer/Camera.h"
#include "Renderer/Material.h"
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

		virtual const RenderStatistics& getRenderStatistics() const = 0;

		virtual void drawLegoPart(const LegoPartComponent& legoPart, const glm::mat4& transform) = 0;
	};

}