#pragma once

#include "Buffer.h"

namespace Brickview
{

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, // For terminal only application maybe
			OpenGL,
		};

	public:
		virtual ~RendererAPI() {}

		static Scope<RendererAPI> create();
		static API getAPI() { return s_api; }

		virtual void init() = 0;
		virtual void shutdown() = 0;

		virtual void setClearColor(float r, float g, float b, float a) = 0;
		virtual void clear() = 0;
		virtual void setViewportDimension(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void drawIndices(const Ref<VertexArray>& vertexArray) = 0;
		virtual void drawInstances(const Ref<VertexArray>& vertexArray, uint32_t instanceCount) = 0;

	private:
		inline static API s_api = API::OpenGL;
	};

}