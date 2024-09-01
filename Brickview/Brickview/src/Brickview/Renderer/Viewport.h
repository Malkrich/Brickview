#pragma once

#include "Buffer/FrameBuffer.h"

namespace Brickview
{

	class Viewport
	{
	public:
		Viewport(uint32_t width, uint32_t height);

		uint32_t getWidth() const { return m_frameBuffer->getSpecifications().Width; }
		uint32_t getHeight() const { return m_frameBuffer->getSpecifications().Height; }

		bool isHovered() const { return m_hovered; }
		bool hasSizeChanged() const { return m_hasSizeChanged; }

		void beginFrame() const { m_frameBuffer->bind(); }
		void endFrame() const { m_frameBuffer->unbind(); }
		void onGuiRender();

	private:
		Scope<FrameBuffer> m_frameBuffer = nullptr;

		bool m_hasSizeChanged = false;
		bool m_hovered = true;
	};

}