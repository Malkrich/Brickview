#include "Pch.h"
#include "SolidRenderer.h"

#include "Renderer/Renderer/RenderQueue.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Buffer/Layout.h"

#include "RenderCommand.h"

namespace Brickview
{

	struct BufferGroup
	{
		std::shared_ptr<VertexArray> Vao = nullptr;
		std::shared_ptr<VertexBuffer> Vbo = nullptr;
		std::shared_ptr<IndexBuffer> Ebo = nullptr;
	};

	struct SolidMeshVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
	};

	struct SolidRendererData
	{
		const unsigned int MaxVertices = 1024;
		const unsigned int MaxIndices = MaxVertices;

		// Mesh
		std::shared_ptr<Shader> MeshShader = nullptr;
		UniformMap MeshUniforms = {};
		BufferGroup MeshBufferGroup;
		std::vector<SolidMeshVertex> MeshVertices;
		std::vector<TriangleFace> MeshIndices;

		// Light
		Light Light;

		// Camera data
		glm::mat4 ViewProjectionMatrix;
		glm::vec3 CameraPosition;
	};

	static SolidRendererData* s_solidRendererData;

	void SolidRenderer::init()
	{
		s_solidRendererData = new SolidRendererData();

		s_solidRendererData->MeshShader = std::make_shared<Shader>("data/shaders/solid.vs", "data/shaders/solid.fs");

		// Mesh
		s_solidRendererData->MeshBufferGroup.Vbo = std::make_shared<VertexBuffer>(s_solidRendererData->MaxVertices);
		Layout meshLayout = {
			{ "a_position", BufferElementType::Float3 },
			{ "a_normal", BufferElementType::Float3 }
		};
		s_solidRendererData->MeshBufferGroup.Vbo->setBufferLayout(meshLayout);
		s_solidRendererData->MeshBufferGroup.Ebo = std::make_shared<IndexBuffer>(s_solidRendererData->MaxIndices);

		s_solidRendererData->MeshBufferGroup.Vao = std::make_shared<VertexArray>();
		s_solidRendererData->MeshBufferGroup.Vao->addVertexBuffer(s_solidRendererData->MeshBufferGroup.Vbo);
		s_solidRendererData->MeshBufferGroup.Vao->setIndexBuffer(s_solidRendererData->MeshBufferGroup.Ebo);
		s_solidRendererData->MeshBufferGroup.Vao->unbind();
	}

	void SolidRenderer::shutdown()
	{
		delete s_solidRendererData;
		s_solidRendererData = nullptr;
	}

	void SolidRenderer::begin(const Camera& camera, const Light& light)
	{
		s_solidRendererData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
		s_solidRendererData->CameraPosition = camera.getPosition();
		s_solidRendererData->Light = light;
	}

	void SolidRenderer::submitMesh(const std::shared_ptr<Mesh>& mesh, const glm::mat4& transform)
	{
		// TODO: auto vertices = mesh->getVertexBuffer(VertexElement::Position | VertexElement::Normal | VertexElement::Color);
		const auto& vertices = mesh->getVertexBuffer();
		const auto& indices = mesh->getIndexBuffer();

		if (vertices.size() + s_solidRendererData->MeshVertices.size() > s_solidRendererData->MaxVertices
			|| indices.size() + s_solidRendererData->MeshIndices.size() > s_solidRendererData->MaxIndices)
		{
			flush();
		}

		unsigned int offset = s_solidRendererData->MeshVertices.size();
		s_solidRendererData->MeshVertices.reserve(s_solidRendererData->MeshVertices.size() + vertices.size());
		s_solidRendererData->MeshIndices.reserve(s_solidRendererData->MeshIndices.size() + indices.size());
		for (const auto& v : vertices)
		{
			glm::vec3 p = transform * glm::vec4(v.Position, 1.0);
			s_solidRendererData->MeshVertices.push_back({ p, v.Normal });
		}
		for (const auto& f : indices)
		{
			TriangleFace face;
			face[0] = f[0] + offset;
			face[1] = f[1] + offset;
			face[2] = f[2] + offset;
			s_solidRendererData->MeshIndices.push_back(face);
		}

		// Update stats
		//s_rendererData->Statistics.MeshVertexCount = s_rendererData->MeshVertices.size();
		//s_rendererData->Statistics.MeshIndicesCount = s_rendererData->MeshIndices.size() * 3;

		s_solidRendererData->MeshUniforms["u_viewProjection"] = s_solidRendererData->ViewProjectionMatrix;
		s_solidRendererData->MeshUniforms["u_cameraPosition"] = s_solidRendererData->CameraPosition;
	}

	void SolidRenderer::flush()
	{
		// Meshes
		if (s_solidRendererData->MeshVertices.size() != 0)
		{
			s_solidRendererData->MeshBufferGroup.Vao->bind();
			s_solidRendererData->MeshBufferGroup.Vbo->setData(
				s_solidRendererData->MeshVertices.size() * sizeof(SolidMeshVertex),
				(void*)s_solidRendererData->MeshVertices.data());
			s_solidRendererData->MeshBufferGroup.Ebo->setData(
				s_solidRendererData->MeshIndices.size() * sizeof(TriangleFace),
				(void*)s_solidRendererData->MeshIndices.data());

			s_solidRendererData->MeshShader->bind();
			s_solidRendererData->MeshShader->setUniforms(s_solidRendererData->MeshUniforms);

			RenderCommand::draw(s_solidRendererData->MeshBufferGroup.Vao);
			//s_rendererData->Statistics.DrawCalls++;
			s_solidRendererData->MeshBufferGroup.Vao->unbind();
			s_solidRendererData->MeshVertices.clear();
			s_solidRendererData->MeshIndices.clear();
		}
	}

	void SolidRenderer::end()
	{
		flush();
	}
}