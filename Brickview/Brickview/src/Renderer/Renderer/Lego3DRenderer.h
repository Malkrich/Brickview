#pragma once

#include "RendererBase.h"
#include "Renderer/Camera.h"
#include "Renderer/Light.h"
#include "Models/Mesh.h"
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
		static void reloadShader(const std::string& shaderName);

		static RenderType getRenderType();
		static void setRenderType(RenderType type);
		static bool isDrawingLights();
		static void drawLights(bool drawLights);

		static void begin(const Camera& camera, const Light& light);
		static void end();
		static void drawMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform);

	private:
		static Scope<RendererBase> rendererFromType(RenderType type);
	};

}