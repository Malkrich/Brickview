#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

	enum class FrameBufferAttachment
	{
		None = 0,

		RedInt,
		RGBA8,

		Depth24Sentil8,

		Depth = Depth24Sentil8
	};

	struct FrameBufferAttachmentSpecs
	{
		FrameBufferAttachment Format = FrameBufferAttachment::None;
		bool Clear = true;

		FrameBufferAttachmentSpecs() = default;
		FrameBufferAttachmentSpecs(FrameBufferAttachment attachment)
			: Format(attachment)
		{}
	};

	struct FrameBufferSpecifications
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		std::vector<FrameBufferAttachmentSpecs> Attachments = {};

		FrameBufferSpecifications() = default;
	};

	class FrameBuffer
	{
	public:
		static Ref<FrameBuffer> create(const FrameBufferSpecifications& specs);

		virtual ~FrameBuffer() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;

		virtual const FrameBufferSpecifications& getSpecifications() const = 0;

		virtual int32_t readPixel(uint32_t attachmentIndex, uint32_t mouseX, uint32_t mouseY) const = 0;

		virtual uint32_t getColorAttachment(uint32_t index) const = 0;
		virtual void clearAttachment(uint32_t attachmentIndex, int value) = 0;
	};

}