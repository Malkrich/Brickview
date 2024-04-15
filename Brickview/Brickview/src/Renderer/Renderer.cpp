#include "Pch.h"
#include "Renderer.h"

#include "Core/Core.h"

#include "RenderCommand.h"

namespace Brickview
{
	struct SceneData
	{
		// Camera
		glm::vec3 CameraPosition;
		glm::mat4 ViewProjectionMatrix;

		// Light
		Light Light;
	};

	static SceneData* s_sceneData = nullptr;

	void Renderer::init()
	{
		RenderCommand::initialise();

		s_sceneData = new SceneData();
	}

	void Renderer::shutdown()
	{
		delete s_sceneData;
	}

	const glm::vec3& Renderer::getLightPosition()
	{
		BV_ASSERT(s_sceneData, "Scene data is null!");
		return s_sceneData->Light.Position;
	}

	void Renderer::begin(const Camera& camera, const Light& light)
	{
		s_sceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
		s_sceneData->CameraPosition = camera.getPosition();
		s_sceneData->Light = light;
	}

	void Renderer::end()
	{
	}

	void Renderer::onWindowResize(unsigned int width, unsigned int height)
	{
		RenderCommand::setViewportDimension(0, 0, width, height);
	}

	void Renderer::onWindowResize(const glm::ivec2& windowDimension)
	{
		onWindowResize(windowDimension.x, windowDimension.y);
	}

	void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
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