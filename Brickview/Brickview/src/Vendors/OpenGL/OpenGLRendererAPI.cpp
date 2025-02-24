#include "Pch.h"
#include "OpenGLRendererAPI.h"

#include "OpenGLError.h"

#include <glad/glad.h>

namespace Brickview
{

	namespace Utils
	{
		static GLenum internalCullModeToOpenGLCullMode(FaceCullingMode mode)
		{
			switch (mode)
			{
				case FaceCullingMode::Front:        return GL_FRONT;
				case FaceCullingMode::Back:         return GL_BACK;
				case FaceCullingMode::FrontAndBack: return GL_FRONT_AND_BACK;
			}

			BV_ASSERT(false, "Unknown face culling mode!");
			return GL_FRONT;
		}

		static GLenum internalFaceWindingModeToOpenGLWindingMode(FaceWindingMode mode)
		{
			switch (mode)
			{
				case FaceWindingMode::Clockwise:        return GL_CW;
				case FaceWindingMode::CounterClockwise: return GL_CCW;
			}

			BV_ASSERT(false, "Unknown face winding mode!");
			return GL_CCW;
		}
	}

	void OpenGLRendererAPI::init()
	{
		enableDepthTesting(true);
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

	void OpenGLRendererAPI::enableDepthTesting(bool enable)
	{
		if (enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::enableFaceCulling(bool enable)
	{
		if (enable)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::setFaceCullingMode(FaceCullingMode mode)
	{
		GLenum glCullMode = Utils::internalCullModeToOpenGLCullMode(mode);
		glCullFace(glCullMode);
	}

	void OpenGLRendererAPI::setFaceWindingMode(FaceWindingMode mode)
	{
		GLenum glFaceWindingMode = Utils::internalFaceWindingModeToOpenGLWindingMode(mode);
		glFrontFace(glFaceWindingMode);
	}

	void OpenGLRendererAPI::drawIndices(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->bind();
		glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::drawInstances(const Ref<VertexArray>& vertexArray, uint32_t instanceCount)
	{
		vertexArray->bind();
		glDrawElementsInstanced(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr, instanceCount);
	}

	void OpenGLRendererAPI::setLineWidth(float lineWidth)
	{
		glLineWidth(lineWidth);
	}

	void OpenGLRendererAPI::drawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::drawLinesIndexed(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->bind();
		glDrawElements(GL_LINES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
	}

}