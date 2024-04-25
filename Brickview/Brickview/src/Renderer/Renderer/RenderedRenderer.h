#pragma once

#include "Models/Mesh.h"

#include "Renderer/Camera.h"
#include "Renderer/Light.h"
#include "Material/Material.h"

namespace Brickview
{
	struct RenderSatistics
	{
		unsigned int DrawCalls = 0;

		// Meshes
		unsigned int MeshVertexCount = 0;
		unsigned int MeshIndicesCount = 0;
	};

	class RenderedRenderer
	{
	public:
		static void init();
		static void shutdown();

		static void begin(const Camera& camera, const Light& light);

		static void drawLights(bool drawLights);
		static void submitMesh(const std::shared_ptr<Mesh>& mesh, const Material& material, const glm::mat4& transform);

		static void end();
		static void flush();

		static const RenderSatistics& getStats();
	private:
		static void submitLight();
	};

}