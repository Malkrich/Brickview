#pragma once

#include "Core/BasicTypes.h"
#include "Renderer/Shader/ShaderLibrary.h"
#include "Renderer/Camera.h"
#include "Renderer/Light.h"
#include "Renderer/Light.h"
#include "Renderer/Material.h"
#include "Mesh/Mesh.h"

#include <glm/glm.hpp>

namespace Brickview
{

	class RenderSystem
	{
	public:
		virtual ~RenderSystem() = default;

		virtual void begin(const Camera& camera, const Light& light) = 0;
		virtual void end() = 0;

		virtual void drawMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform) {}
		virtual void drawLights(const Light& light) {}
	};

}