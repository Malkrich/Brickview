#include "Pch.h"
#include "Renderer.h"

#include "Core/Core.h"

#include "RenderCommand.h"

namespace Brickview
{
	struct SceneData
	{
		glm::mat4 ViewProjectionMatrix;

		glm::vec3 LightPosition;
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
		return s_sceneData->LightPosition;
	}

	void Renderer::begin(const Camera& camera, const glm::vec3& lightPosition)
	{
		s_sceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
		s_sceneData->LightPosition = lightPosition;
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
		shader->setMat4("u_viewProjection", s_sceneData->ViewProjectionMatrix);
		shader->setVec3("u_lightPosition", s_sceneData->LightPosition);
		shader->setMat4("u_transform", transform);
		vertexArray->bind();

		RenderCommand::draw(vertexArray);
	}

}