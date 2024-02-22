#include "Pch.h"
#include "Renderer.h"

#include "RenderCommand.h"

namespace Brickview
{
	struct SceneData
	{
		glm::mat4 ViewProjectionMatrix;
	};

	static SceneData* s_sceneData = nullptr;

	void Renderer::init()
	{
		s_sceneData = new SceneData();
	}

	void Renderer::shutdown()
	{
		delete s_sceneData;
	}

	void Renderer::begin(const Camera& camera)
	{
		s_sceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::onWindowResize(unsigned int width, unsigned int height)
	{
		RenderCommand::setViewportDimension(0, 0, width, height);
	}

	void Renderer::onWindowResize(const glm::ivec2& windowDimension)
	{
		onWindowResize(windowDimension.x, windowDimension.y);
	}

	void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{
		shader->bind();
		shader->setMat4("u_viewProjection", s_sceneData->ViewProjectionMatrix);
		vertexArray->bind();

		RenderCommand::draw(vertexArray);
	}

}