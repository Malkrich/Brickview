#pragma once

#include "RenderSystem.h"
#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Shader/ShaderLibrary.h"
#include "Renderer/Camera.h"
#include "Mesh/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/Light.h"

#include <glm/glm.hpp>

namespace Brickview
{

	struct InstanceSubmission
	{
		InstanceSubmission(Ref<VertexArray> vao, uint32_t instanceCount)
			: Vao(vao)
			, InstanceCount(instanceCount)
		{}

		Ref<VertexArray> Vao;
		uint32_t InstanceCount;
	};

	class SolidRenderSystem : public RenderSystem
	{
	public:
		SolidRenderSystem(const Ref<ShaderLibrary>& shaderLib);
		virtual ~SolidRenderSystem();

		virtual void begin(const Camera& camera, const Light& light) override;
		virtual void end() override;

		virtual void drawMeshes(const Ref<Mesh>& mesh, const Material& material, const std::vector<glm::mat4>& transforms) override;

	private:
		Ref<Shader> m_solidShader;

		std::vector<InstanceSubmission> m_submissions;

		// Camera data
		glm::mat4 m_viewProjectionMatrix;
		glm::vec3 m_cameraPosition;
	};

}