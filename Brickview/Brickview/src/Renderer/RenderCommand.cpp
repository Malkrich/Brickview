#include "Pch.h"
#include "RenderCommand.h"

#include <glad/glad.h>

namespace Brickview
{

	void RenderCommand::setClearColor(float r, float g, float b, float a)
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
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