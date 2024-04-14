#include "Pch.h"
#include "LegoRenderer.h"

#include "Renderer/Renderer.h"
#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Buffer/Layout.h"
#include "Renderer/Shader/Shader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

namespace Brickview
{
	struct LegoVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Color;
	};

	struct RenderData
	{
		std::shared_ptr<Shader> LegoShader;
		std::shared_ptr<Shader> LightShader;
	};

	static RenderData* s_data;

	void LegoRenderer::init()
	{
		s_data = new RenderData();

		// Lego Shader
		s_data->LegoShader.reset(new Shader("data/shaders/legoPiece.vs", "data/shaders/legoPiece.fs"));
		s_data->LightShader.reset(new Shader("data/shaders/light.vs", "data/shaders/light.fs"));
	}

	void LegoRenderer::shutdown()
	{
		delete s_data;
	}

	void LegoRenderer::drawPiece(std::shared_ptr<Mesh> mesh, const Material& material, const glm::mat4& transform)
	{
		const auto& vertices = mesh->getVertexBuffer();
		const auto& indices = mesh->getIndexBuffer();
		std::vector<LegoVertex> legoVertices;
		legoVertices.reserve(vertices.size());
		for (const auto& v : vertices)
			legoVertices.push_back({ v.Position, v.Normal, material.Color });

		std::shared_ptr<VertexArray> vao = std::make_shared<VertexArray>();

		std::shared_ptr<VertexBuffer> vbo = std::make_shared<VertexBuffer>(legoVertices.size() * sizeof(LegoVertex), (void*)legoVertices.data());
		Layout pieceLayout = {
			{ "a_position", BufferElementType::Float3 },
			{ "a_normal", BufferElementType::Float3 },
			{ "a_color", BufferElementType::Float3 }
		};
		vbo->setBufferLayout(pieceLayout);
		vao->addVertexBuffer(vbo);

		std::shared_ptr<IndexBuffer> ebo = std::make_shared<IndexBuffer>(indices.size() * sizeof(TriangleFace), (void*)indices.data());
		vao->setIndexBuffer(ebo);

		Renderer::submit(s_data->LegoShader, vao, transform);
	}

	void LegoRenderer::drawLight()
	{
		const glm::vec3& lightPosition = Renderer::getLightPosition();
		static float scaleFactor = 0.2f;
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), lightPosition) * glm::scale(glm::mat4(1.0f), scaleFactor*glm::vec3(1.0f));

		std::shared_ptr<Mesh> lightMesh = Mesh::load("data/models/cube.obj");

		const auto& vertices = lightMesh->getVertexBuffer();
		const auto& indices = lightMesh->getIndexBuffer();
		std::vector<glm::vec3> lightVertices;
		lightVertices.reserve(vertices.size());
		for (const auto& v : vertices)
			lightVertices.push_back(v.Position);

		std::shared_ptr<VertexArray> vao = std::make_shared<VertexArray>();

		std::shared_ptr<VertexBuffer> vbo = std::make_shared<VertexBuffer>(lightVertices.size() * sizeof(glm::vec3), (void*)lightVertices.data());
		Layout pieceLayout = { { "a_position", BufferElementType::Float3 } };
		vbo->setBufferLayout(pieceLayout);
		vao->addVertexBuffer(vbo);

		std::shared_ptr<IndexBuffer> ebo = std::make_shared<IndexBuffer>(indices.size() * sizeof(TriangleFace), (void*)indices.data());
		vao->setIndexBuffer(ebo);

		Renderer::submit(s_data->LightShader, vao, transform);
	}

}