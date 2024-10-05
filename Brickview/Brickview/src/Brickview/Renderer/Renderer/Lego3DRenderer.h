#pragma once

#include "RenderSystem.h"
#include "Renderer/Camera.h"
#include "Renderer/Light.h"
#include "Mesh/Mesh.h"
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
		static void drawMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform);
		static void drawMeshes(const Ref<Mesh>& mesh, const Material& material, const std::vector<glm::mat4>& transforms);
	};

}