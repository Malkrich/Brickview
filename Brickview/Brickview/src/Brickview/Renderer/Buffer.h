#pragma once

#include "Layout.h"

namespace Brickview
{
	class VertexBuffer
	{
	public:
		static Ref<VertexBuffer> create(uint32_t size, const void* data);
		static Ref<VertexBuffer> create(uint32_t size);

		virtual ~VertexBuffer() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void setData(uint32_t size, void* data) = 0;

		virtual const Layout& getLayout() const = 0;
		virtual void setBufferLayout(const Layout& layout) = 0;
	};

	class IndexBuffer
	{
	public:
		static Ref<IndexBuffer> create(uint32_t size, const void* data);
		static Ref<IndexBuffer> create(uint32_t size);

		virtual ~IndexBuffer() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual uint32_t getCount() const = 0;

		virtual void setData(uint32_t size, void* data) = 0;
	};

	class VertexArray
	{
	public:
		static Ref<VertexArray> create();

		virtual ~VertexArray() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void setIndexBuffer(const  Ref<IndexBuffer>& indexBuffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() = 0;
		virtual const Ref<IndexBuffer>& getIndexBuffer() const = 0;
	};
}