#pragma once

#include "Renderer/RendererAPI.h"

namespace Brickview
{

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void init() override;
		virtual void shutdown() override;

		// Clearing
		virtual void setClearColor(float r, float g, float b, float a) override;
		virtual void clear() override;
		virtual void setViewportDimension(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		// Elements
		virtual void drawIndices(const Ref<VertexArray>& vertexArray) override;
		virtual void drawInstances(const Ref<VertexArray>& vertexArray, uint32_t instanceCount) override;

		// Lines
		virtual void setLineWidth(float lineWidth) override;
		virtual void drawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
	};

}