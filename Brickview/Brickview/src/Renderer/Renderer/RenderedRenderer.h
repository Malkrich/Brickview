#pragma once

#include "Renderer/Shader/Shader.h"
#include "Renderer/Camera.h"
#include "Models/Mesh.h"
#include "Renderer/Light.h"
#include "Renderer/Material.h"

namespace Brickview
{
	struct RenderSatistics
	{
		uint32_t DrawCalls = 0;

		// Meshes
		uint32_t MeshVertexCount = 0;
		uint32_t MeshIndicesCount = 0;
	};

	class RenderedRenderer
	{
	public:
		static void init(const Ref<Shader>& meshShader, const Ref<Shader>& lightShader);
		static void shutdown();

		static void begin(const Camera& camera, const Light& light);

		static void submitMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform);
		static void submitLights();

		static void end();
		static void flush();

		static const RenderSatistics& getStats();

		static void setMeshShader(const Ref<Shader>& shader);
		static void setLightShader(const Ref<Shader>& shader);
	};

}