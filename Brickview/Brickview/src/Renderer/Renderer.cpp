#include "Pch.h"
#include "Renderer.h"

#include "RenderCommand.h"

namespace Brickview
{

	void Renderer::init() {}
	void Renderer::shutdown() {}

	void Renderer::begin()
	{

	}

	void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{
		shader->bind();
		vertexArray->bind();

		RenderCommand::draw(vertexArray);
	}

}