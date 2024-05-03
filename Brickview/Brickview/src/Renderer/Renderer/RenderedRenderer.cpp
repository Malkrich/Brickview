#include "Pch.h"
#include "RenderedRenderer.h"

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

	static RenderedRendererData* s_renderedRendererData = nullptr;

	void RenderedRenderer::init()
	{
		s_renderedRendererData = new RenderedRendererData();

		s_renderedRendererData->MeshShader = std::make_shared<Shader>("data/shaders/legoPiece.vs", "data/shaders/legoPiece.fs");
		s_renderedRendererData->LightShader = std::make_shared<Shader>("data/shaders/light.vs", "data/shaders/light.fs");

		// Mesh
		s_renderedRendererData->MeshBufferGroup.Vbo = std::make_shared<VertexBuffer>(s_renderedRendererData->MaxVertices);
		Layout meshLayout = {
			{ "a_position", BufferElementType::Float3 },
			{ "a_normal", BufferElementType::Float3 },
			{ "a_color", BufferElementType::Float3 }
		};
		s_renderedRendererData->MeshBufferGroup.Vbo->setBufferLayout(meshLayout);
		s_renderedRendererData->MeshBufferGroup.Ebo = std::make_shared<IndexBuffer>(s_renderedRendererData->MaxIndices);

		s_renderedRendererData->MeshBufferGroup.Vao = std::make_shared<VertexArray>();
		s_renderedRendererData->MeshBufferGroup.Vao->addVertexBuffer(s_renderedRendererData->MeshBufferGroup.Vbo);
		s_renderedRendererData->MeshBufferGroup.Vao->setIndexBuffer(s_renderedRendererData->MeshBufferGroup.Ebo);
		s_renderedRendererData->MeshBufferGroup.Vao->unbind();

		// Light
		s_renderedRendererData->LightBufferGroup.Vbo = std::make_shared<VertexBuffer>(s_renderedRendererData->MaxVertices);
		Layout lightLayout = {
			{ "a_position", BufferElementType::Float3 }
		};
		s_renderedRendererData->LightBufferGroup.Vbo->setBufferLayout(lightLayout);
		s_renderedRendererData->LightBufferGroup.Ebo = std::make_shared<IndexBuffer>(s_renderedRendererData->MaxIndices);

		s_renderedRendererData->LightBufferGroup.Vao = std::make_shared<VertexArray>();
		s_renderedRendererData->LightBufferGroup.Vao->addVertexBuffer(s_renderedRendererData->LightBufferGroup.Vbo);
		s_renderedRendererData->LightBufferGroup.Vao->setIndexBuffer(s_renderedRendererData->LightBufferGroup.Ebo);
		s_renderedRendererData->LightBufferGroup.Vao->unbind();

		s_renderedRendererData->LightMesh = Mesh::load("data/models/cube.obj");
	}

	void RenderedRenderer::shutdown()
	{
		delete s_renderedRendererData;
		s_renderedRendererData = nullptr;
	}

	void RenderedRenderer::begin(const Camera& camera, const Light& light)
	{
		s_renderedRendererData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
		s_renderedRendererData->CameraPosition = camera.getPosition();
		s_renderedRendererData->Light = light;

		s_renderedRendererData->Statistics.DrawCalls = 0;

		if(s_renderedRendererData->DrawLights)
			submitLight();

	}

	void RenderedRenderer::drawLights(bool drawLights)
	{
		s_renderedRendererData->DrawLights = drawLights;
	}

	void RenderedRenderer::submitMesh(const std::shared_ptr<Mesh>& mesh, const Material& material, const glm::mat4& transform)
	{
		// TODO: auto vertices = mesh->getVertexBuffer(VertexElement::Position | VertexElement::Normal | VertexElement::Color);
		const auto& vertices = mesh->getVertexBuffer();
		const auto& indices = mesh->getIndexBuffer();

		if (vertices.size() + s_renderedRendererData->MeshVertices.size() > s_renderedRendererData->MaxVertices
			|| indices.size() + s_renderedRendererData->MeshIndices.size() > s_renderedRendererData->MaxIndices)
		{
			flush();
		}

		unsigned int offset = s_renderedRendererData->MeshVertices.size();
		s_renderedRendererData->MeshVertices.reserve(s_renderedRendererData->MeshVertices.size() + vertices.size());
		s_renderedRendererData->MeshIndices.reserve(s_renderedRendererData->MeshIndices.size() + indices.size());
		for (const auto& v : vertices)
		{
			glm::vec3 p = transform * glm::vec4(v.Position, 1.0);
			s_renderedRendererData->MeshVertices.push_back({ p, v.Normal, material.Color });
		}
		for (const auto& f : indices)
		{
			TriangleFace face;
			face[0] = f[0] + offset;
			face[1] = f[1] + offset;
			face[2] = f[2] + offset;
			s_renderedRendererData->MeshIndices.push_back(face);
		}
		
		// Update stats
		s_renderedRendererData->Statistics.MeshVertexCount = s_renderedRendererData->MeshVertices.size();
		s_renderedRendererData->Statistics.MeshIndicesCount = s_renderedRendererData->MeshIndices.size() * 3;

		s_renderedRendererData->MeshUniforms["u_viewProjection"] = s_renderedRendererData->ViewProjectionMatrix;
		s_renderedRendererData->MeshUniforms["u_cameraPosition"] = s_renderedRendererData->CameraPosition;
		s_renderedRendererData->MeshUniforms["u_lightPosition"] = s_renderedRendererData->Light.Position;
		s_renderedRendererData->MeshUniforms["u_lightColor"] = s_renderedRendererData->Light.Color;

		// TODO
		//RenderQueue::submit(s_rendererData->MeshVertexArray, s_rendererData->MeshShader, uniforms);
	}

	void RenderedRenderer::submitLight()
	{
		static const float lightScale = 0.1f;
		glm::mat4 lightTransform = glm::translate(glm::mat4(1.0f), s_renderedRendererData->Light.Position)
			* glm::scale(glm::mat4(1.0f), lightScale * glm::vec3(1.0f));

		const auto& vertices = s_renderedRendererData->LightMesh->getVertexBuffer();
		const auto& indices = s_renderedRendererData->LightMesh->getIndexBuffer();

		unsigned int offset = s_renderedRendererData->LightVertices.size();
		s_renderedRendererData->LightVertices.reserve(s_renderedRendererData->LightVertices.size() + vertices.size());
		s_renderedRendererData->LightIndices.reserve(s_renderedRendererData->LightIndices.size() + indices.size());
		for (const auto& v : vertices)
		{
			glm::vec3 p = lightTransform * glm::vec4(v.Position, 1.0f);
			s_renderedRendererData->LightVertices.push_back({ p });
		}
		for (const auto& f : indices)
		{
			TriangleFace face;
			face[0] = f[0] + offset;
			face[1] = f[1] + offset;
			face[2] = f[2] + offset;
			s_renderedRendererData->LightIndices.push_back(face);
		}

		s_renderedRendererData->LightUniforms["u_viewProjection"] = s_renderedRendererData->ViewProjectionMatrix;
		s_renderedRendererData->LightUniforms["u_lightColor"] = s_renderedRendererData->Light.Color;

		// TODO
		//RenderQueue::submit(s_rendererData->LightVertexArray, s_rendererData->LightShader, uniforms);
	}

	void RenderedRenderer::flush()
	{
		// Meshes
		if (s_renderedRendererData->MeshVertices.size() != 0)
		{
			s_renderedRendererData->MeshBufferGroup.Vao->bind();
			s_renderedRendererData->MeshBufferGroup.Vbo->setData(
				s_renderedRendererData->MeshVertices.size() * sizeof(MeshVertex),
				(void*)s_renderedRendererData->MeshVertices.data());
			s_renderedRendererData->MeshBufferGroup.Ebo->setData(
				s_renderedRendererData->MeshIndices.size() * sizeof(TriangleFace),
				(void*)s_renderedRendererData->MeshIndices.data());

			s_renderedRendererData->MeshShader->bind();
			s_renderedRendererData->MeshShader->setUniforms(s_renderedRendererData->MeshUniforms);

			RenderCommand::draw(s_renderedRendererData->MeshBufferGroup.Vao);
			s_renderedRendererData->Statistics.DrawCalls++;
			s_renderedRendererData->MeshBufferGroup.Vao->unbind();
			s_renderedRendererData->MeshVertices.clear();
			s_renderedRendererData->MeshIndices.clear();
		}

		// Lights
		if (s_renderedRendererData->LightVertices.size() != 0)
		{
			s_renderedRendererData->LightBufferGroup.Vao->bind();
			s_renderedRendererData->LightBufferGroup.Vbo->setData(
				s_renderedRendererData->LightVertices.size() * sizeof(LightVertex),
				(void*)s_renderedRendererData->LightVertices.data());
			s_renderedRendererData->LightBufferGroup.Ebo->setData(
				s_renderedRendererData->LightIndices.size() * sizeof(TriangleFace),
				(void*)s_renderedRendererData->LightIndices.data());

			s_renderedRendererData->LightShader->bind();
			s_renderedRendererData->LightShader->setUniforms(s_renderedRendererData->LightUniforms);

			RenderCommand::draw(s_renderedRendererData->LightBufferGroup.Vao);
			s_renderedRendererData->Statistics.DrawCalls++;
			s_renderedRendererData->LightBufferGroup.Vao->unbind();
			s_renderedRendererData->LightVertices.clear();
			s_renderedRendererData->LightIndices.clear();
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
		return s_renderedRendererData->Statistics;
	}
}