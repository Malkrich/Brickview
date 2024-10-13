#include "Pch.h"
#include "SolidRenderSystem.h"
#include "Renderer/Shader/ShaderLibrary.h"
#include "Renderer/Buffer/Layout.h"

#include "Renderer/Renderer/RenderCommand.h"

namespace Brickview
{

	SolidRenderSystem::SolidRenderSystem(const Ref<ShaderLibrary>& shaderLib)
		: m_solidShader(shaderLib->get("Solid"))
	{}

	SolidRenderSystem::~SolidRenderSystem() {}

	void SolidRenderSystem::begin(const Camera& camera, const Light& light)
	{
		m_viewProjectionMatrix = camera.getViewProjectionMatrix();
		m_cameraPosition       = camera.getPosition();

		m_uniforms["u_viewProjection"] = m_viewProjectionMatrix;
		m_uniforms["u_cameraPosition"] = m_cameraPosition;
	}

	void SolidRenderSystem::drawMesh(const Ref<GpuMesh>& mesh, const Material& material, const glm::mat4& transform)
	{
	}

	void SolidRenderSystem::flush(const InstanceData& instanceData)
	{
		// Transform buffer
		const TransformBuffer& instanceTransforms = instanceData.InstanceTransforms;
		uint32_t transformBufferSize = instanceData.InstanceCount * sizeof(glm::mat4);

		Ref<VertexBuffer> meshTransformBuffer = createRef<VertexBuffer>(
			transformBufferSize,
			(void*)instanceTransforms.data());
		Layout meshTransformLayout = {
			{ 2, "a_transform", BufferElementType::Mat4 }
		};
		meshTransformBuffer->setBufferLayout(meshTransformLayout);

		Ref<VertexArray> vao = createRef<VertexArray>();
		vao->addGeometry(instanceData.Mesh);
		vao->addVertexBuffer(meshTransformBuffer);

		m_solidShader->bind();
		m_solidShader->setUniforms(m_uniforms);

		RenderCommand::drawInstances(vao, instanceData.InstanceCount);

		vao->unbind();
	}

	void SolidRenderSystem::end()
	{
		for (const auto& element : m_instanceRegistry)
		{
			const InstanceData& instanceData = element.second;
			flush(instanceData);
		}
	}

}