#pragma once

#include "Renderer/Buffer/Buffer.h"

namespace Brickview
{

	class RenderCommand
	{
	public:
		static void setClearColor(float r, float g, float b, float a);
		static void clear();

		static void draw(const std::shared_ptr<VertexArray>& vertexArray);
	};

}