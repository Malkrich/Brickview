#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

	struct FrameBufferSpecifications
	{
		uint32_t Width = 1280, Height = 720;
	};

	class FrameBuffer
	{
	public:
		FrameBuffer(const FrameBufferSpecifications& spec);
		~FrameBuffer();

		void bind() const;
		void unbind() const;

		void resize(uint32_t width, uint32_t height);

		const FrameBufferSpecifications& getSpecifications() const { return m_spec; }

		uint32_t getColorAttachment() const { return m_colorAttachment; }

	private:
		void invalidate();

	private:
		FrameBufferSpecifications m_spec;

		uint32_t m_bufferID = 0;
		uint32_t m_colorAttachment = 0;
		uint32_t m_depthAttachment = 0;
	};

}