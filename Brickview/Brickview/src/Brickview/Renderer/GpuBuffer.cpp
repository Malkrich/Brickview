#include "Pch.h"
#include "GpuBuffer.h"

#include "RendererAPI.h"
#include "Vendors/OpenGL/OpenGLBuffer.h"

namespace Brickview
{

	namespace Utils
	{
		using BufferElementType = Brickview::BufferElementType;

		static uint32_t getElementSize(BufferElementType type)
		{
			switch (type)
			{
			case BufferElementType::Bool:	return 1;
			case BufferElementType::Int:	return 1 * sizeof(int);
			case BufferElementType::Int2:	return 2 * sizeof(int);
			case BufferElementType::Int3:	return 3 * sizeof(int);
			case BufferElementType::Int4:	return 4 * sizeof(int);
			case BufferElementType::Float:	return 1 * sizeof(float);
			case BufferElementType::Float2: return 2 * sizeof(float);
			case BufferElementType::Float3: return 3 * sizeof(float);
			case BufferElementType::Float4: return 4 * sizeof(float);
			case BufferElementType::Mat2:	return 2 * 2 * sizeof(float);
			case BufferElementType::Mat3:	return 3 * 3 * sizeof(float);
			case BufferElementType::Mat4:	return 4 * 4 * sizeof(float);
			}

			BV_ASSERT(false, "Buffer element type unknown !");
			return 0;
		}
	}

	Layout::Layout(const std::initializer_list<BufferElement>& elements)
		: m_elements(elements)
	{
		computeOffsetAndStride();
	}

	void Layout::computeOffsetAndStride()
	{
		uint32_t offset = 0;
		for (auto& element : m_elements)
		{
			element.Offset = offset;
			uint32_t elementSize = Utils::getElementSize(element.Type);
			m_stride += elementSize;
			offset += elementSize;
		}
	}

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