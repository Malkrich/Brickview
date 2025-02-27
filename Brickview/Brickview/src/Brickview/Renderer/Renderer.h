#pragma once

#include "Renderer/Shader/ShaderLibrary.h"
#include "Shader/Shader.h"
#include "RendererMaterial.h"
#include "Lights.h"
#include "GpuMesh.h"
#include "Primitives.h"
#include "CubeMap.h"
#include "Texture2D.h"

#include <glm/glm.hpp>

namespace Brickview
{

	struct CameraData
	{
		glm::mat4 ViewProjectionMatrix;
		glm::vec3 Position;
	};

	struct RendererEnvironment
	{
		std::vector<PointLight> PointLights;
		std::vector<int> PointLightIDs;

	};

	class Renderer
	{
	public:
		static void init();
		static void shutdown();

		// Read only
		static const Ref<ShaderLibrary>& getShaderLibrary();

		// Setup environment
		static void begin(const CameraData& cameraData, const RendererEnvironment& env);
		static Ref<CubeMap> createCubeMap(Ref<Texture2D> hdriTexture);

		// Meshes
		static void renderMesh(Ref<Shader> shader, const RendererMaterial& material, const Ref<GpuMesh>& mesh, const glm::mat4& transform, int entityID);
		static void renderMeshInstances(Ref<Shader> shader, const Ref<GpuMesh>& mesh, const void* instanceBufferData, const Layout& instanceBufferLayout, uint32_t instanceBufferSize, size_t instanceCount);
		static void renderMeshWireframe(Ref<GpuMesh> mesh, const glm::mat4& transform, float lineWidth);

		// Lights
		static void renderPointLights();

		// Lines
		static void renderLines(const std::vector<Line>& lines, const glm::vec3& color, float lineWidth);
		static void renderLines(const std::vector<Line>& lines, const std::vector<glm::vec3>& colors, float lineWidth);
	};

}