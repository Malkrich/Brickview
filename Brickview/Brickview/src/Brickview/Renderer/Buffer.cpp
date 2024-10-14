#include "Pch.h"
#include "Buffer.h"

#include "RendererAPI.h"
#include "Vendors/OpenGL/OpenGLBuffer.h"

namespace Brickview
{

	/*********************************************************/
	/*************   VERTEX BUFFER   *************************/
	/*********************************************************/

	Ref<VertexBuffer> VertexBuffer::create(uint32_t size, const void* data)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLVertexBuffer>(size, data);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}
	
	Ref<VertexBuffer> VertexBuffer::create(uint32_t size)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLVertexBuffer>(size);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

	/*********************************************************/
	/*************   INDEX BUFFER   **************************/
	/*********************************************************/

	Ref<IndexBuffer> IndexBuffer::create(uint32_t size, const void* data)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLIndexBuffer>(size, data);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::create(uint32_t size)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLIndexBuffer>(size);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

	/*********************************************************/
	/*************   VERTEX ARRAY   **************************/
	/*********************************************************/

	Ref<VertexArray> VertexArray::create()
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLVertexArray>();
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

}