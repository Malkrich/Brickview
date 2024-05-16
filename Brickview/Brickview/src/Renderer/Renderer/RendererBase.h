#pragma once

#include "Renderer/Shader/ShaderLibrary.h"
#include "Renderer/Camera.h"
#include "Renderer/Light.h"
#include "Renderer/Light.h"
#include "Renderer/Material.h"
#include "Models/Mesh.h"

#include <glm/glm.hpp>

namespace Brickview
{

	class RendererBase
	{
	public:
		virtual ~RendererBase() = default;

		virtual void begin(const Camera& camera, const Light& light) = 0;
		virtual void end() = 0;

		virtual void drawMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform) = 0;
		virtual void drawLights(const Light& light) = 0;
	};

}