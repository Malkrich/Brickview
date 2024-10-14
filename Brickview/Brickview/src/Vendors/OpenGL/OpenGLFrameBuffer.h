#pragma once

#include "Renderer/FrameBuffer.h"

namespace Brickview
{

	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecifications& specs);

		virtual ~OpenGLFrameBuffer();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void resize(uint32_t width, uint32_t height) override;

		virtual const FrameBufferSpecifications& getSpecifications() const override { return m_specs; }

		virtual uint32_t getColorAttachment() const override { return m_colorAttachment; }

	private:
		void invalidate();

	private:
		FrameBufferSpecifications m_specs;

		uint32_t m_bufferID;
		uint32_t m_colorAttachment;
		uint32_t m_depthAttachment;
	};

}