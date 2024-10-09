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
	}

	void SolidRenderSystem::drawMeshes(const Ref<Mesh>& mesh, const Material& material, const std::vector<glm::mat4>& transforms)
	{
		const std::vector<Vertex>& meshData = mesh->getVertices();
		const std::vector<TriangleFace>& meshConnectivities = mesh->getConnectivities();

		Ref<VertexBuffer> meshVertexBuffer = createRef<VertexBuffer>(
			meshData.size() * sizeof(Vertex),
			(void*)meshData.data());
		Layout meshGeoLayout = {
			{ "a_position", BufferElementType::Float3 },
			{ "a_normal", BufferElementType::Float3 } };
		meshVertexBuffer->setBufferLayout(meshGeoLayout);

		Ref<IndexBuffer> meshIndexBuffer = createRef<IndexBuffer>(
			meshConnectivities.size() * sizeof(TriangleFace),
			(void*)meshConnectivities.data());

		Ref<VertexBuffer> meshTransformBuffer = createRef<VertexBuffer>(
			transforms.size() * sizeof(glm::mat4),
			(void*)transforms.data());
		Layout meshTransformLayout = {
			{ 2, "a_transform", BufferElementType::Mat4 }
		};
		meshTransformBuffer->setBufferLayout(meshTransformLayout);

		Ref<VertexArray> vao = createRef<VertexArray>();
		vao->addVertexBuffer(meshVertexBuffer);
		vao->setIndexBuffer(meshIndexBuffer);
		vao->addVertexBuffer(meshTransformBuffer);

		m_submissions.push_back({ vao, transforms.size() });
		vao->unbind();
	}

	void SolidRenderSystem::end()
	{
		UniformMap uniforms;
		uniforms["u_viewProjection"] = m_viewProjectionMatrix;
		uniforms["u_cameraPosition"] = m_cameraPosition;

		m_solidShader->bind();
		m_solidShader->setUniforms(uniforms);

		for (const auto& submission : m_submissions)
			RenderCommand::drawInstances(submission.Vao, submission.InstanceCount);
			
		m_submissions.clear();
	}

}