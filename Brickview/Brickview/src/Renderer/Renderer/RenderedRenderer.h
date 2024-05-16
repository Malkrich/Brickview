#pragma once

#include "RendererBase.h"
#include "Renderer/Renderer/BatchRendererManager.h"
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

	namespace RenderedRendererTypes
	{

		struct MeshVertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec3 Color;
		};

		struct LightVertex
		{
			glm::vec3 Position;
		};

	}
	
	class RenderedRenderer : public RendererBase
	{
	public:
		RenderedRenderer(const Ref<ShaderLibrary>& shaderLib);
		virtual ~RenderedRenderer();

		virtual void begin(const Camera& camera, const Light& light) override;
		virtual void end() override;

		virtual void drawMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform) override;
		virtual void drawLights(const Light& light) override;

		//const RenderSatistics& getStats();

	private:
		void flush();

	private:
		const uint32_t m_maxVertices = 1024;
		const uint32_t m_maxIndices = m_maxVertices;

		// Mesh
		UniformMap m_meshUniforms = {};
		std::vector<RenderedRendererTypes::MeshVertex> m_meshVertices;
		std::vector<TriangleFace> m_meshIndices;

		// Light
		UniformMap m_lightUniforms = {};
		std::vector<RenderedRendererTypes::LightVertex> m_lightVertices;
		std::vector<TriangleFace> m_lightIndices;
		Ref<Mesh> m_lightMesh = nullptr;

		Light m_light;

		// Camera
		glm::mat4 m_viewProjectionMatrix;
		glm::vec3 m_cameraPosition;

		// Stats
		RenderSatistics m_statistics;

		// Queue
		Scope<BatchRendererManager> m_rendererManager = nullptr;
	};

}