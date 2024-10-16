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

		virtual uint32_t getColorAttachment(uint32_t index) const override;

	private:
		void invalidate();

	private:
		uint32_t m_bufferID;

		FrameBufferSpecifications m_specs;

		std::vector<FrameBufferAttachmentSpecs> m_colorAttachmentsSpecs = {};
		FrameBufferAttachmentSpecs m_depthAttachmentSpecs;

		std::vector<uint32_t> m_colorAttachments = {};
		uint32_t m_depthAttachment = 0;
	};

}