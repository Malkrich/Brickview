#include "Pch.h"
#include "FrameBuffer.h"

#include "RendererAPI.h"
#include "Vendors/OpenGL/OpenGLFrameBuffer.h"

namespace Brickview
{

	Ref<FrameBuffer> FrameBuffer::create(const FrameBufferSpecifications& specs)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLFrameBuffer>(specs);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

}