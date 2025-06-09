#pragma once

#include "Renderer/Shader/ShaderLibrary.h"
#include "Shader/Shader.h"
#include "RendererMaterial.h"
#include "Lights.h"
#include "GpuMesh.h"
#include "Primitives.h"
#include "Cubemap.h"
#include "Texture2D.h"

#include <glm/glm.hpp>

namespace Brickview
{

	struct CameraData
	{
		glm::mat4 ViewProjectionMatrix;
		glm::mat4 View;
		glm::mat4 Projection;
		glm::vec3 Position;

		CameraData() = default;
	};

	struct CubemapsCreationInfo
	{
		uint32_t EnvironmentMapDimXY = 512;
		uint32_t IrradianceMapDimXY = 512;
		uint32_t PreFilteredMapDimXY = 128;
		uint32_t PreFilteredMapMipMapLevelCount = 5;
		uint32_t BrdfLUTMapDimXY = 512;

		CubemapsCreationInfo() = default;
	};

	struct CubemapTextures
	{
		Ref<Cubemap> EnvironmentMap = nullptr;
		Ref<Cubemap> IrradianceMap = nullptr;
		Ref<Cubemap> PreFilteredEnvMap = nullptr;

		CubemapTextures() = default;
	};

	struct RendererEnvironment
	{
		std::vector<PointLight> PointLights = {};
		std::vector<int> PointLightIDs = {};

		CubemapTextures EnvironmentCubemaps;
		Ref<Cubemap> RenderedSkyboxHandle = nullptr;
		float RenderedSkyboxMipLevel;

		RendererEnvironment() = default;
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
		static void end();

		// Cubemap / Skybox
		static CubemapTextures createEnvironmentCubemaps(Ref<Texture2D> hdriTexture, const CubemapsCreationInfo& cubemapsCreationInfo);
		static void renderSkybox(Ref<Cubemap> cubemap);

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