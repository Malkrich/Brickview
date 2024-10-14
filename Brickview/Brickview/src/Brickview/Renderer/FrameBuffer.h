#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

	struct FrameBufferSpecifications
	{
		uint32_t Width;
		uint32_t Height;

		FrameBufferSpecifications() = default;
		FrameBufferSpecifications(uint32_t width, uint32_t height)
			: Width(width)
			, Height(height)
		{}
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

		virtual uint32_t getColorAttachment() const = 0;
	};

}