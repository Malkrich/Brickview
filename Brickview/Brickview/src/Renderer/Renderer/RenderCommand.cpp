#include "Pch.h"
#include "RenderCommand.h"

#include <glad/glad.h>

namespace Brickview
{

	void RenderCommand::init()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void RenderCommand::setClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}
	void RenderCommand::setClearColor(float r, float g, float b)
	{
		setClearColor(r, g, b, 1.0f);
	}

	void RenderCommand::setClearColor(const glm::vec3& clearColor)
	{
		setClearColor(clearColor.r, clearColor.g, clearColor.b);
	}

	void RenderCommand::setClearColor(const glm::vec4& clearColor)
	{
		setClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	}

	void RenderCommand::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RenderCommand::setViewportDimension(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void RenderCommand::draw(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->bind();
		glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
	}

}