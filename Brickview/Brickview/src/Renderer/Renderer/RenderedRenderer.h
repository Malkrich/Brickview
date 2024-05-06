#pragma once

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
		static void init();
		static void shutdown();

		static void begin(const Camera& camera, const Light& light);

		static void drawLights(bool drawLights);
		static void submitMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform);

		static void end();
		static void flush();

		static const RenderSatistics& getStats();

	private:
		static void submitLight();
	};

}