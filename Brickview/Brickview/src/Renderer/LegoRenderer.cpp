#include "Pch.h"
#include "LegoRenderer.h"

#include "Core/Core.h"
#include "RenderCommand.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>>
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
		// Global
		RenderType RenderType = RenderType::Rendered;

		// Camera
		glm::vec3 CameraPosition;
		glm::mat4 ViewProjectionMatrix;

		// Light
		Light Light;

		// Shaders
		std::shared_ptr<Shader> SolidShader;
		std::shared_ptr<Shader> LegoShader;
		std::shared_ptr<Shader> LightShader;
	};

	static SceneData* s_rendererData = nullptr;

	void LegoRenderer::init()
	{
		RenderCommand::initialise();

		s_rendererData = new SceneData();

		// Lego Shader
		loadShaders();
	}

	void LegoRenderer::loadShaders()
	{
		s_rendererData->SolidShader.reset(new Shader("data/shaders/solid.vs", "data/shaders/solid.fs"));
		s_rendererData->LegoShader.reset(new Shader("data/shaders/legoPiece.vs", "data/shaders/legoPiece.fs"));
		s_rendererData->LightShader.reset(new Shader("data/shaders/light.vs", "data/shaders/light.fs"));
	}

	void LegoRenderer::shutdown()
	{
		delete s_rendererData;
	}

	void LegoRenderer::begin(const Camera& camera, const Light& light)
	{
		s_rendererData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
		s_rendererData->CameraPosition = camera.getPosition();
		s_rendererData->Light = light;
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

	void LegoRenderer::setRenderType(RenderType type)
	{
		s_rendererData->RenderType = type;
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

		switch (s_rendererData->RenderType)
		{
			case RenderType::Solid:
				LegoRenderer::submit(s_rendererData->SolidShader, vao, transform);
				break;
			case RenderType::Rendered:
				LegoRenderer::submit(s_rendererData->LegoShader, vao, transform);
				break;
		}
	}

	void LegoRenderer::drawLight()
	{
		static float scaleFactor = 0.1f;
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), s_rendererData->Light.Position) * glm::scale(glm::mat4(1.0f), scaleFactor * glm::vec3(1.0f));

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

		LegoRenderer::submit(s_rendererData->LightShader, vao, transform);
	}

	void LegoRenderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->bind();
		UniformMap uniforms;
		uniforms["u_viewProjection"] = UniformData(s_rendererData->ViewProjectionMatrix);
		uniforms["u_transform"] = UniformData(transform);
		uniforms["u_cameraPosition"] = UniformData(s_rendererData->CameraPosition);
		uniforms["u_lightPosition"] = UniformData(s_rendererData->Light.Position);
		uniforms["u_lightColor"] = UniformData(s_rendererData->Light.Color);
		shader->setUniforms(uniforms);

		vertexArray->bind();

		RenderCommand::draw(vertexArray);
	}

}