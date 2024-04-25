#include "Pch.h"
#include "RenderedRenderer.h"

#include "Renderer/Renderer/RenderQueue.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Buffer/Layout.h"

#include "Renderer/RenderCommand.h"

namespace Brickview
{
	struct BufferGroup
	{
		std::shared_ptr<VertexArray> Vao = nullptr;
		std::shared_ptr<VertexBuffer> Vbo = nullptr;
		std::shared_ptr<IndexBuffer> Ebo = nullptr;
	};

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
	
	struct RenderedRendererData
	{
		const unsigned int MaxVertices = 1024;
		const unsigned int MaxIndices = MaxVertices;

		// Mesh
		std::shared_ptr<Shader> MeshShader = nullptr;
		UniformMap MeshUniforms = {};
		BufferGroup MeshBufferGroup;
		std::vector<MeshVertex> MeshVertices;
		std::vector<TriangleFace> MeshIndices;

		// Light
		std::shared_ptr<Shader> LightShader = nullptr;
		UniformMap LightUniforms = {};
		BufferGroup LightBufferGroup;
		std::vector<LightVertex> LightVertices;
		std::vector<TriangleFace> LightIndices;
		std::shared_ptr<Mesh> LightMesh;
		bool DrawLights = false;
		Light Light;

		// Camera
		glm::mat4 ViewProjectionMatrix;
		glm::vec3 CameraPosition;

		// Stats
		RenderSatistics Statistics;
	};

	static RenderedRendererData* s_rendererData = nullptr;

	void RenderedRenderer::init()
	{
		s_rendererData = new RenderedRendererData();

		s_rendererData->MeshShader = std::make_shared<Shader>("data/shaders/legoPiece.vs", "data/shaders/legoPiece.fs");
		s_rendererData->LightShader = std::make_shared<Shader>("data/shaders/light.vs", "data/shaders/light.fs");

		// Mesh
		s_rendererData->MeshBufferGroup.Vbo = std::make_shared<VertexBuffer>(s_rendererData->MaxVertices);
		Layout meshLayout = {
			{ "a_position", BufferElementType::Float3 },
			{ "a_normal", BufferElementType::Float3 },
			{ "a_color", BufferElementType::Float3 }
		};
		s_rendererData->MeshBufferGroup.Vbo->setBufferLayout(meshLayout);
		s_rendererData->MeshBufferGroup.Ebo = std::make_shared<IndexBuffer>(s_rendererData->MaxIndices);

		s_rendererData->MeshBufferGroup.Vao = std::make_shared<VertexArray>();
		s_rendererData->MeshBufferGroup.Vao->addVertexBuffer(s_rendererData->MeshBufferGroup.Vbo);
		s_rendererData->MeshBufferGroup.Vao->setIndexBuffer(s_rendererData->MeshBufferGroup.Ebo);
		s_rendererData->MeshBufferGroup.Vao->unbind();

		// Light
		s_rendererData->LightBufferGroup.Vbo = std::make_shared<VertexBuffer>(s_rendererData->MaxVertices);
		Layout lightLayout = {
			{ "a_position", BufferElementType::Float3 }
		};
		s_rendererData->LightBufferGroup.Vbo->setBufferLayout(lightLayout);
		s_rendererData->LightBufferGroup.Ebo = std::make_shared<IndexBuffer>(s_rendererData->MaxIndices);

		s_rendererData->LightBufferGroup.Vao = std::make_shared<VertexArray>();
		s_rendererData->LightBufferGroup.Vao->addVertexBuffer(s_rendererData->LightBufferGroup.Vbo);
		s_rendererData->LightBufferGroup.Vao->setIndexBuffer(s_rendererData->LightBufferGroup.Ebo);
		s_rendererData->LightBufferGroup.Vao->unbind();

		s_rendererData->LightMesh = Mesh::load("data/models/cube.obj");
	}

	void RenderedRenderer::shutdown()
	{
		delete s_rendererData;
		s_rendererData = nullptr;
	}

	void RenderedRenderer::begin(const Camera& camera, const Light& light)
	{
		s_rendererData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
		s_rendererData->CameraPosition = camera.getPosition();
		s_rendererData->Light = light;

		s_rendererData->Statistics.DrawCalls = 0;

		if(s_rendererData->DrawLights)
			submitLight();

	}

	void RenderedRenderer::drawLights(bool drawLights)
	{
		s_rendererData->DrawLights = drawLights;
	}

	void RenderedRenderer::submitMesh(const std::shared_ptr<Mesh>& mesh, const Material& material, const glm::mat4& transform)
	{
		// TODO: auto vertices = mesh->getVertexBuffer(VertexElement::Position | VertexElement::Normal | VertexElement::Color);
		const auto& vertices = mesh->getVertexBuffer();
		const auto& indices = mesh->getIndexBuffer();

		if (vertices.size() + s_rendererData->MeshVertices.size() > s_rendererData->MaxVertices
			|| indices.size() + s_rendererData->MeshIndices.size() > s_rendererData->MaxIndices)
		{
			flush();
		}

		unsigned int offset = s_rendererData->MeshVertices.size();
		s_rendererData->MeshVertices.reserve(s_rendererData->MeshVertices.size() + vertices.size());
		s_rendererData->MeshIndices.reserve(s_rendererData->MeshIndices.size() + indices.size());
		for (const auto& v : vertices)
		{
			glm::vec3 p = transform * glm::vec4(v.Position, 1.0);
			s_rendererData->MeshVertices.push_back({ p, v.Normal, material.Color });
		}
		for (const auto& f : indices)
		{
			TriangleFace face;
			face[0] = f[0] + offset;
			face[1] = f[1] + offset;
			face[2] = f[2] + offset;
			s_rendererData->MeshIndices.push_back(face);
		}
		
		// Update stats
		s_rendererData->Statistics.MeshVertexCount = s_rendererData->MeshVertices.size();
		s_rendererData->Statistics.MeshIndicesCount = s_rendererData->MeshIndices.size() * 3;

		s_rendererData->MeshUniforms["u_viewProjection"] = s_rendererData->ViewProjectionMatrix;
		s_rendererData->MeshUniforms["u_cameraPosition"] = s_rendererData->CameraPosition;
		s_rendererData->MeshUniforms["u_lightPosition"] = s_rendererData->Light.Position;
		s_rendererData->MeshUniforms["u_lightColor"] = s_rendererData->Light.Color;

		// TODO
		//RenderQueue::submit(s_rendererData->MeshVertexArray, s_rendererData->MeshShader, uniforms);
	}

	void RenderedRenderer::submitLight()
	{
		static const float lightScale = 0.1f;
		glm::mat4 lightTransform = glm::translate(glm::mat4(1.0f), s_rendererData->Light.Position)
			* glm::scale(glm::mat4(1.0f), lightScale * glm::vec3(1.0f));

		const auto& vertices = s_rendererData->LightMesh->getVertexBuffer();
		const auto& indices = s_rendererData->LightMesh->getIndexBuffer();

		unsigned int offset = s_rendererData->LightVertices.size();
		s_rendererData->LightVertices.reserve(s_rendererData->LightVertices.size() + vertices.size());
		s_rendererData->LightIndices.reserve(s_rendererData->LightIndices.size() + indices.size());
		for (const auto& v : vertices)
		{
			glm::vec3 p = lightTransform * glm::vec4(v.Position, 1.0f);
			s_rendererData->LightVertices.push_back({ p });
		}
		for (const auto& f : indices)
		{
			TriangleFace face;
			face[0] = f[0] + offset;
			face[1] = f[1] + offset;
			face[2] = f[2] + offset;
			s_rendererData->LightIndices.push_back(face);
		}

		s_rendererData->LightUniforms["u_viewProjection"] = s_rendererData->ViewProjectionMatrix;
		s_rendererData->LightUniforms["u_lightColor"] = s_rendererData->Light.Color;

		// TODO
		//RenderQueue::submit(s_rendererData->LightVertexArray, s_rendererData->LightShader, uniforms);
	}

	void RenderedRenderer::flush()
	{
		// Meshes
		if (s_rendererData->MeshVertices.size() != 0)
		{
			s_rendererData->MeshBufferGroup.Vao->bind();
			s_rendererData->MeshBufferGroup.Vbo->setData(
				s_rendererData->MeshVertices.size() * sizeof(MeshVertex),
				(void*)s_rendererData->MeshVertices.data());
			s_rendererData->MeshBufferGroup.Ebo->setData(
				s_rendererData->MeshIndices.size() * sizeof(TriangleFace),
				(void*)s_rendererData->MeshIndices.data());

			s_rendererData->MeshShader->bind();
			s_rendererData->MeshShader->setUniforms(s_rendererData->MeshUniforms);

			RenderCommand::draw(s_rendererData->MeshBufferGroup.Vao);
			s_rendererData->Statistics.DrawCalls++;
			s_rendererData->MeshBufferGroup.Vao->unbind();
			s_rendererData->MeshVertices.clear();
			s_rendererData->MeshIndices.clear();
		}

		// Lights
		if (s_rendererData->LightVertices.size() != 0)
		{
			s_rendererData->LightBufferGroup.Vao->bind();
			s_rendererData->LightBufferGroup.Vbo->setData(
				s_rendererData->LightVertices.size() * sizeof(LightVertex),
				(void*)s_rendererData->LightVertices.data());
			s_rendererData->LightBufferGroup.Ebo->setData(
				s_rendererData->LightIndices.size() * sizeof(TriangleFace),
				(void*)s_rendererData->LightIndices.data());

			s_rendererData->LightShader->bind();
			s_rendererData->LightShader->setUniforms(s_rendererData->LightUniforms);

			RenderCommand::draw(s_rendererData->LightBufferGroup.Vao);
			s_rendererData->Statistics.DrawCalls++;
			s_rendererData->LightBufferGroup.Vao->unbind();
			s_rendererData->LightVertices.clear();
			s_rendererData->LightIndices.clear();
		}
	}

	void RenderedRenderer::end()
	{
		flush();
		// TODO
		//RenderQueue::flush();
	}

	const RenderSatistics& RenderedRenderer::getStats()
	{
		return s_rendererData->Statistics;
	}
}