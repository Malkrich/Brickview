#include "Pch.h"
#include "SolidRenderSystem.h"

#include "Renderer/Buffer/Buffer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Shader/ShaderLibrary.h"
#include "Renderer/Buffer/Layout.h"
#include "Lego/LegoMeshRegistry.h"

namespace Brickview
{

	using TransformBuffer = std::array<glm::mat4, 1000>;

	struct InstanceData
	{
		Ref<GpuMesh> Mesh = nullptr;
		TransformBuffer InstanceTransforms;
		size_t InstanceCount = 0;
	};

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

		RenderCommand::setClearColor(0.2f, 0.2f, 0.2f);
		RenderCommand::clear();

		resetStats();
	}

	void SolidRenderSystem::drawLegoPart(const LegoPartComponent& legoPart, const glm::mat4& transform)
	{
		bool exists = m_instanceRegistry.contains(legoPart.ID);
		auto& instanceData = m_instanceRegistry[legoPart.ID];

		if (!exists)
		{
			const LegoPartMeshData& meshData = legoPart.PartRegistry->getPart(legoPart.ID);
			instanceData.Mesh = meshData.Mesh;
		}

		instanceData.InstanceTransforms[instanceData.InstanceCount] = transform;
		instanceData.InstanceCount++;

		// stats
		if (m_renderStatistics.MaxInstanceCount < instanceData.InstanceCount)
			m_renderStatistics.MaxInstanceCount = instanceData.InstanceCount;

		if (instanceData.InstanceCount == instanceData.InstanceTransforms.size())
		{
			flush(instanceData);
			instanceData.InstanceCount = 0;
		}

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
		vao->addVertexBuffer(instanceData.Mesh->getGeometryVertexBuffer());
		vao->setIndexBuffer(instanceData.Mesh->getGeometryIndexBuffer());
		vao->addVertexBuffer(meshTransformBuffer);

		m_solidShader->bind();
		m_solidShader->setUniforms(m_uniforms);

		RenderCommand::drawInstances(vao, instanceData.InstanceCount);
		m_renderStatistics.DrawCalls++;

		vao->unbind();
	}

	void SolidRenderSystem::end()
	{
		for (const auto& element : m_instanceRegistry)
		{
			const InstanceData& instanceData = element.second;
			if (instanceData.InstanceCount != 0)
				flush(instanceData);
		}

		m_instanceRegistry.clear();
	}

	void SolidRenderSystem::resetStats()
	{
		m_renderStatistics = RenderStatistics();
	}

}