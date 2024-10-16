#include "Pch.h"
#include "Viewport.h"

#include <imgui.h>

namespace Brickview
{

	Viewport::Viewport(uint32_t width, uint32_t height)
	{
		FrameBufferSpecifications specs;
		specs.Width   = width;
		specs.Height  = height;
		m_frameBuffer = FrameBuffer::create(specs);
	}

	void Viewport::onGuiRender()
	{
	}

}