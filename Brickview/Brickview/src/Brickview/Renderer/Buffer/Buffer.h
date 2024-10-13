#pragma once

#include "Layout.h"

namespace Brickview
{
	class VertexBuffer
	{
	public:
		VertexBuffer(uint32_t size, const void* data);
		VertexBuffer(uint32_t size);

		~VertexBuffer();

		void bind() const;
		void unbind() const;

		void setData(uint32_t size, void* data);

		const Layout& getLayout() const { return m_layout; }
		void setBufferLayout(const Layout& layout) { m_layout = layout; }

	private:
		uint32_t m_bufferID;
		Layout m_layout;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t size, const void* data);
		IndexBuffer(uint32_t size);

		~IndexBuffer();

		void bind() const;
		void unbind() const;
		uint32_t getCount() const { return m_count; }

		void setData(uint32_t size, void* data);

	private:
		uint32_t m_bufferID;
		uint32_t m_count;
	};

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void bind() const;
		void unbind() const;

		void addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
		void setIndexBuffer(const  Ref<IndexBuffer>& indexBuffer);

		const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const { return m_vertexBuffers; }
		const Ref<IndexBuffer>& getIndexBuffer() const { return m_indexBuffer; }

	private:
		unsigned int m_bufferID;

		std::vector<Ref<VertexBuffer>> m_vertexBuffers;
		Ref<IndexBuffer> m_indexBuffer;
	};
}