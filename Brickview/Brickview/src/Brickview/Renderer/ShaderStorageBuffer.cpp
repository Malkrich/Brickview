#include "Pch.h"
#include "ShaderStorageBuffer.h"

#include "RendererAPI.h"
#include "Vendors/OpenGL/OpenGLShaderStorageBuffer.h"

namespace Brickview
{

	Ref<ShaderStorageBuffer> ShaderStorageBuffer::create(const ShaderStorageBufferSpecifications& specs, uint32_t size, const void* data)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!"); return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLShaderStorageBuffer>(specs, size, data);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

	Ref<ShaderStorageBuffer> ShaderStorageBuffer::create(const ShaderStorageBufferSpecifications& specs, uint32_t size)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!"); return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLShaderStorageBuffer>(specs, size);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

}