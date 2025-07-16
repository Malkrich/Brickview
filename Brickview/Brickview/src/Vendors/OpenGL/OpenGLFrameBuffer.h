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

		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }

		virtual const FrameBufferAttachmentSpecs& getColorAttachmentSpecs(uint32_t attachmentIndex) const override { return m_colorAttachmentsSpecs[attachmentIndex]; }
		virtual const FrameBufferAttachmentSpecs& getDepthAttachmentSpecs() const override { return m_depthAttachmentSpecs; }

		virtual int32_t readPixel(uint32_t attachmentIndex, uint32_t mouseX, uint32_t mouseY) const override;

		virtual uint32_t getColorAttachment(uint32_t index) const override;
		virtual void setColorAttachment(uint32_t index, Ref<Texture> texture) override;
		virtual void clearAttachment(uint32_t attachmentIndex, int value) override;

		virtual void attachCubemapFace(uint32_t attachmentIndex, CubemapFace face, uint32_t mipmapLevel = 0) override;

	private:
		void invalidate();

		void populateAttachments(const std::vector<FrameBufferAttachmentSpecs>& attachments);
		void updateSpecsFromTexture(FrameBufferAttachmentSpecs& attachmentSpecs);

	private:
		uint32_t m_bufferID = 0;

		uint32_t m_width, m_height;

		// Color attachments
		std::vector<FrameBufferAttachmentSpecs> m_colorAttachmentsSpecs = {};
		std::vector<uint32_t> m_colorAttachments = {};

		// Depth attachment
		FrameBufferAttachmentSpecs m_depthAttachmentSpecs = ColorAttachmentFormat::None;
		uint32_t m_depthAttachment = 0;
	};

}