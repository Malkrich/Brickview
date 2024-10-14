#include "Pch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Brickview
{

	void OpenGLRendererAPI::init()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::shutdown()
	{
	}

	void OpenGLRendererAPI::setClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void OpenGLRendererAPI::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::setViewportDimension(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::drawIndices(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->bind();
		glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::drawInstances(const Ref<VertexArray>& vertexArray, uint32_t instanceCount)
	{
		vertexArray->bind();
		glDrawElementsInstanced(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, 0, instanceCount);
	}

}