#pragma once

#include "RendererBase.h"
#include "Renderer/Renderer/BatchRendererManager.h"
#include "Renderer/Shader/ShaderLibrary.h"
#include "Renderer/Camera.h"
#include "Models/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/Light.h"

#include <glm/glm.hpp>

namespace Brickview
{

	namespace SolidRendererTypes
	{
		struct MeshVertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
		};
	}

	class SolidRenderer : public RendererBase
	{
	public:
		SolidRenderer(const Ref<ShaderLibrary>& shaderLib);
		virtual ~SolidRenderer();

		virtual const Ref<RenderSettings>& getRenderSettings() const { return m_settings; }

		virtual void begin(const Camera& camera, const Light& light) override;
		virtual void end() override;

		virtual void drawMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform) override;
		virtual void drawLights(const Light& light) override;

	private:
		void flush();

	private:
		const uint32_t m_maxVertices = 1024;
		const uint32_t m_maxIndices = m_maxVertices;

		// Mesh
		UniformMap m_meshUniforms = {};
		std::vector<SolidRendererTypes::MeshVertex> m_meshVertices;
		std::vector<TriangleFace> m_meshIndices;

		// Light
		Light m_light;

		// Camera data
		glm::mat4 m_viewProjectionMatrix;
		glm::vec3 m_cameraPosition;

		// Settings
		bool m_showNormals = false;
		Ref<RenderSettings> m_settings;

		// Render submissions
		Scope<BatchRendererManager> m_rendererManager = nullptr;
	};

}