#include "Pch.h"
#include "RenderCommand.h"

#include <glad/glad.h>

namespace Brickview
{

	void RenderCommand::setClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void RenderCommand::setClearColor(const glm::vec3& clearColor)
	{
		setClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
	}

	void RenderCommand::setClearColor(const glm::vec4& clearColor)
	{
		setClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	}

	void RenderCommand::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void RenderCommand::draw(const std::shared_ptr<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
	}

}