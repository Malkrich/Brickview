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

		virtual int32_t readPixel(uint32_t attachmentIndex, uint32_t mouseX, uint32_t mouseY) const override;

		virtual uint32_t getColorAttachment(uint32_t index) const override;
		virtual void clearAttachment(uint32_t attachmentIndex, int value) override;

	private:
		void invalidate();

	private:
		uint32_t m_bufferID = 0;

		FrameBufferSpecifications m_specs;

		std::vector<FrameBufferAttachmentSpecs> m_colorAttachmentsSpecs = {};
		FrameBufferAttachmentSpecs m_depthAttachmentSpecs = FrameBufferAttachment::None;

		std::vector<uint32_t> m_colorAttachments = {};
		uint32_t m_depthAttachment = 0;
	};

}