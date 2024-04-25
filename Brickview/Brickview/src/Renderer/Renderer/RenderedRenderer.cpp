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
	};

	static RenderedRendererData* s_rendererData = nullptr;

	void RenderedRenderer::init()
	{
		s_rendererData = new RenderedRendererData();

		s_rendererData->MeshShader = std::make_shared<Shader>("data/shaders/legoPiece.vs", "data/shaders/legoPiece.fs");
		s_rendererData->LightShader = std::make_shared<Shader>("data/shaders/light.vs", "data/shaders/light.fs");

		// Mesh
		s_rendererData->MeshBufferGroup.Vbo = std::make_shared<VertexBuffer>(1024);
		Layout meshLayout = {
			{ "a_position", BufferElementType::Float3 },
			{ "a_normal", BufferElementType::Float3 },
			{ "a_color", BufferElementType::Float3 }
		};
		s_rendererData->MeshBufferGroup.Vbo->setBufferLayout(meshLayout);
		s_rendererData->MeshBufferGroup.Ebo = std::make_shared<IndexBuffer>(1024);

		s_rendererData->MeshBufferGroup.Vao = std::make_shared<VertexArray>();
		s_rendererData->MeshBufferGroup.Vao->addVertexBuffer(s_rendererData->MeshBufferGroup.Vbo);
		s_rendererData->MeshBufferGroup.Vao->setIndexBuffer(s_rendererData->MeshBufferGroup.Ebo);
		s_rendererData->MeshBufferGroup.Vao->unbind();

		// Light
		s_rendererData->LightBufferGroup.Vbo = std::make_shared<VertexBuffer>(1024);
		Layout lightLayout = {
			{ "a_position", BufferElementType::Float3 }
		};
		s_rendererData->LightBufferGroup.Vbo->setBufferLayout(lightLayout);
		s_rendererData->LightBufferGroup.Ebo = std::make_shared<IndexBuffer>(1024);

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
		std::vector<MeshVertex> legoVertices;
		legoVertices.reserve(vertices.size());
		for (const auto& v : vertices)
		{
			glm::vec3 p = transform * glm::vec4(v.Position, 1.0f);
			legoVertices.push_back({ p, v.Normal, material.Color });
		}

		s_rendererData->MeshVertices = legoVertices;
		s_rendererData->MeshIndices = indices;

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
		std::vector<LightVertex> lightVertices;
		lightVertices.reserve(vertices.size());
		for (const auto& v : vertices)
		{
			glm::vec3 p = lightTransform * glm::vec4(v.Position, 1.0);
			lightVertices.push_back({ p });
		}

		s_rendererData->LightVertices = lightVertices;
		s_rendererData->LightIndices = indices;

		s_rendererData->LightUniforms["u_viewProjection"] = s_rendererData->ViewProjectionMatrix;
		s_rendererData->LightUniforms["u_lightColor"] = s_rendererData->Light.Color;

		// TODO
		//RenderQueue::submit(s_rendererData->LightVertexArray, s_rendererData->LightShader, uniforms);
	}

	void RenderedRenderer::end()
	{
		// Meshes
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
		s_rendererData->MeshBufferGroup.Vao->unbind();

		// Lights
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
		s_rendererData->LightBufferGroup.Vao->unbind();

		// TODO
		//RenderQueue::flush();
	}

}