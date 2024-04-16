#include "Pch.h"
#include "LegoRenderer.h"

#include "Core/Core.h"
#include "RenderCommand.h"

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

	struct SceneData
	{
		// Camera
		glm::vec3 CameraPosition;
		glm::mat4 ViewProjectionMatrix;

		// Light
		Light Light;

		// Shaders
		std::shared_ptr<Shader> LegoShader;
		std::shared_ptr<Shader> LightShader;
	};

	static SceneData* s_sceneData = nullptr;

	void LegoRenderer::init()
	{
		RenderCommand::initialise();

		s_sceneData = new SceneData();

		// Lego Shader
		s_sceneData->LegoShader.reset(new Shader("data/shaders/legoPiece.vs", "data/shaders/legoPiece.fs"));
		s_sceneData->LightShader.reset(new Shader("data/shaders/light.vs", "data/shaders/light.fs"));
	}

	void LegoRenderer::shutdown()
	{
		delete s_sceneData;
	}

	void LegoRenderer::begin(const Camera& camera, const Light& light)
	{
		s_sceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
		s_sceneData->CameraPosition = camera.getPosition();
		s_sceneData->Light = light;
	}

	void LegoRenderer::end()
	{
	}

	void LegoRenderer::onWindowResize(unsigned int width, unsigned int height)
	{
		RenderCommand::setViewportDimension(0, 0, width, height);
	}

	void LegoRenderer::onWindowResize(const glm::ivec2& windowDimension)
	{
		onWindowResize(windowDimension.x, windowDimension.y);
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

		LegoRenderer::submit(s_sceneData->LegoShader, vao, transform);
	}

	void LegoRenderer::drawLight()
	{
		static float scaleFactor = 0.1f;
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), s_sceneData->Light.Position) * glm::scale(glm::mat4(1.0f), scaleFactor * glm::vec3(1.0f));

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

		LegoRenderer::submit(s_sceneData->LightShader, vao, transform);
	}

	void LegoRenderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->bind();
		// M V P
		shader->setMat4("u_viewProjection", s_sceneData->ViewProjectionMatrix);
		shader->setMat4("u_transform", transform);
		shader->setVec3("u_cameraPosition", s_sceneData->CameraPosition);
		// Light
		shader->setVec3("u_lightPosition", s_sceneData->Light.Position);
		shader->setVec3("u_lightColor", s_sceneData->Light.Color);

		vertexArray->bind();

		RenderCommand::draw(vertexArray);
	}

}