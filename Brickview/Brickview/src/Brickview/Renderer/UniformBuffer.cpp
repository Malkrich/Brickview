#include "Pch.h"
#include "UniformBuffer.h"

#include "Renderer/RendererAPI.h"
#include "Vendors/OpenGL/OpenGLUniformBuffer.h"

#include <glm/glm.hpp>

namespace Brickview
{

	Ref<UniformBuffer> UniformBuffer::create(const UniformBufferSpecifications& specs, uint32_t size)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLUniformBuffer>(specs, size);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;

	}

	Ref<UniformBuffer> UniformBuffer::create(const UniformBufferSpecifications& specs, uint32_t size, const void* data)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLUniformBuffer>(specs, size, data);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

}