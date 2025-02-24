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

		// Pipeline
		virtual void setViewportDimension(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void enableDepthTesting(bool enable) override;
		virtual void enableFaceCulling(bool enable) override;
		virtual void setFaceCullingMode(FaceCullingMode mode) override;
		virtual void setFaceWindingMode(FaceWindingMode mode) override;
		virtual void setPolygonMode(PolygonMode mode) override;

		// Elements
		virtual void drawIndexed(const Ref<VertexArray>& vertexArray) override;
		virtual void drawInstances(const Ref<VertexArray>& vertexArray, uint32_t instanceCount) override;

		// Lines
		virtual void setLineWidth(float lineWidth) override;
		virtual void drawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
		virtual void drawLinesIndexed(const Ref<VertexArray>& vertexArray) override;
	};

}