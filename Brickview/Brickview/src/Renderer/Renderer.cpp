#include "Pch.h"
#include "Renderer.h"

#include "RenderCommand.h"

namespace Brickview
{

	void Renderer::init() {}
	void Renderer::shutdown() {}

	void Renderer::begin() {}

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
		vertexArray->bind();

		RenderCommand::draw(vertexArray);
	}

}