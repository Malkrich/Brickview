#include "Pch.h"
#include "RenderCommand.h"

namespace Brickview
{

	Scope<RendererAPI> RenderCommand::s_rendererAPI = nullptr;

	void RenderCommand::init()
	{
		s_rendererAPI = RendererAPI::create();
		s_rendererAPI->init();
	}

	void RenderCommand::shutdown()
	{
		s_rendererAPI->shutdown();
	}

	void RenderCommand::setClearColor(float r, float g, float b, float a)
	{
		s_rendererAPI->setClearColor(r, g, b, a);
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
		s_rendererAPI->clear();
	}

	void RenderCommand::setViewportDimension(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		s_rendererAPI->setViewportDimension(x, y, width, height);
	}

	void RenderCommand::setViewportDimension(uint32_t width, uint32_t height)
	{
		RenderCommand::setViewportDimension(0, 0, width, height);
	}

	void RenderCommand::setViewportDimension(uint32_t dimXY)
	{
		RenderCommand::setViewportDimension(dimXY, dimXY);
	}

	void RenderCommand::enableDepthTesting(bool enable)
	{
		s_rendererAPI->enableDepthTesting(enable);
	}

	void RenderCommand::setDepthFunction(DepthFunction function)
	{
		s_rendererAPI->setDepthfunction(function);
	}

	void RenderCommand::enableFaceCulling(bool enable)
	{
		s_rendererAPI->enableFaceCulling(enable);
	}

	void RenderCommand::setFaceCullingMode(FaceCullingMode mode)
	{
		s_rendererAPI->setFaceCullingMode(mode);
	}

	void RenderCommand::setFaceWindingMode(FaceWindingMode mode)
	{
		s_rendererAPI->setFaceWindingMode(mode);
	}

	void RenderCommand::setPolygonMode(PolygonMode mode)
	{
		s_rendererAPI->setPolygonMode(mode);
	}

	void RenderCommand::enableCubemapSeamless(bool enable)
	{
		s_rendererAPI->enableCubemapSeamless(enable);
	}

	void RenderCommand::drawIndexed(const Ref<VertexArray>& vertexArray)
	{
		s_rendererAPI->drawIndexed(vertexArray);
	}

	void RenderCommand::drawInstances(const Ref<VertexArray>& vertexArray, uint32_t instanceCount)
	{
		s_rendererAPI->drawInstances(vertexArray, instanceCount);
	}

	void RenderCommand::setLineWidth(float lineWidth)
	{
		s_rendererAPI->setLineWidth(lineWidth);
	}

	void RenderCommand::drawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		s_rendererAPI->drawLines(vertexArray, vertexCount);
	}

	void RenderCommand::drawLinesIndexed(const Ref<VertexArray>& vertexArray)
	{
		s_rendererAPI->drawLinesIndexed(vertexArray);
	}

}