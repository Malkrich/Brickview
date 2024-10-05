#include "Pch.h"
#include "SolidRenderSystem.h"
#include "Renderer/Shader/ShaderLibrary.h"
#include "Renderer/Buffer/Layout.h"

#include "Renderer/Renderer/RenderCommand.h"

// TEMP
#include <glad/glad.h>

namespace Brickview
{

	SolidRenderSystem::SolidRenderSystem(const Ref<ShaderLibrary>& shaderLib)
		: m_systemShader(shaderLib->get("Solid"))
	{}

	SolidRenderSystem::~SolidRenderSystem()
	{
	}

	void SolidRenderSystem::begin(const Camera& camera, const Light& light)
	{
		m_viewProjectionMatrix = camera.getViewProjectionMatrix();
		m_cameraPosition       = camera.getPosition();
	}

	void SolidRenderSystem::drawMeshes(const Ref<Mesh>& mesh, const Material& material, const std::vector<glm::mat4>& transforms)
	{
		const std::vector<Vertex>& meshData = mesh->getVertices();
		const std::vector<TriangleFace>& meshConnectivities = mesh->getConnectivities();

		m_meshVertexBuffer = createRef<VertexBuffer>(
			meshData.size() * sizeof(Vertex),
			(void*)meshData.data());
		Layout meshGeoLayout = {
			{ "a_position", BufferElementType::Float3 },
			{ "a_normal", BufferElementType::Float3 }};
		m_meshVertexBuffer->setBufferLayout(meshGeoLayout);

		m_meshIndexBuffer = createRef<IndexBuffer>(
			meshConnectivities.size() * sizeof(TriangleFace),
			(void*)meshConnectivities.data());

		m_meshTransformBuffer = createRef<VertexBuffer>(
			transforms.size() * sizeof(glm::mat4),
			(void*)transforms.data()
		);
		Layout meshTransformLayout = {
			{ 2, "a_transform", BufferElementType::Mat4 }
		};
		m_meshTransformBuffer->setBufferLayout(meshTransformLayout);
		
		m_meshVertexArray = createRef<VertexArray>();
		m_meshVertexArray->addVertexBuffer(m_meshVertexBuffer);
		m_meshVertexArray->setIndexBuffer(m_meshIndexBuffer);
		m_meshVertexArray->addVertexBuffer(m_meshTransformBuffer);
	}

	void SolidRenderSystem::end()
	{
		UniformMap uniforms;
		uniforms["u_viewProjection"] = m_viewProjectionMatrix;
		uniforms["u_cameraPosition"] = m_cameraPosition;

		m_systemShader->bind();
		m_systemShader->setUniforms(uniforms);

		m_meshVertexArray->bind();

		//RenderCommand::drawInstances(m_meshVao);
	}

}