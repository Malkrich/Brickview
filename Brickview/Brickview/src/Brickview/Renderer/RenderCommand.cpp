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

	void RenderCommand::drawIndices(const Ref<VertexArray>& vertexArray)
	{
		s_rendererAPI->drawIndices(vertexArray);
	}

	void RenderCommand::drawInstances(const Ref<VertexArray>& vertexArray, uint32_t instanceCount)
	{
		s_rendererAPI->drawInstances(vertexArray, instanceCount);
	}

}