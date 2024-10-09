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

	class SolidRenderSystem : public RenderSystem
	{
	public:
		SolidRenderSystem(const Ref<ShaderLibrary>& shaderLib);
		virtual ~SolidRenderSystem();

		virtual void begin(const Camera& camera, const Light& light) override;
		virtual void end() override;

		virtual void drawMeshes(const Ref<Mesh>& mesh, const Material& material, const std::vector<glm::mat4>& transforms);

	private:
		Ref<Shader> m_systemShader;

		Ref<VertexArray> m_meshVertexArray;
		Ref<VertexBuffer> m_meshVertexBuffer;
		Ref<IndexBuffer> m_meshIndexBuffer;
		Ref<VertexBuffer> m_meshTransformBuffer;
		uint32_t m_instanceCount;

		// Camera data
		glm::mat4 m_viewProjectionMatrix;
		glm::vec3 m_cameraPosition;
	};

}