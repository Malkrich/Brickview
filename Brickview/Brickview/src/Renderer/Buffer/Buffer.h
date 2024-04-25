#pragma once

#include "Layout.h"

namespace Brickview
{
	class VertexBuffer
	{
	public:
		VertexBuffer(unsigned int size, const void* data);
		VertexBuffer(unsigned int size);

		~VertexBuffer();

		void bind() const;
		void unbind() const;

		void setData(unsigned int size, void* data);

		const Layout& getLayout() const { return m_layout; }
		void setBufferLayout(const Layout& layout) { m_layout = layout; }

	private:
		unsigned int m_bufferID;
		Layout m_layout;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer(unsigned int size, const void* data);
		IndexBuffer(unsigned int size);

		~IndexBuffer();

		void bind() const;
		void unbind() const;
		unsigned int getCount() const { return m_count; }

		void setData(unsigned int size, void* data);

	private:
		unsigned int m_count;
		unsigned int m_bufferID;
	};

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void bind() const;
		void unbind() const;

		void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void setIndexBuffer(const  std::shared_ptr<IndexBuffer>& indexBuffer);

		const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const { return m_vertexBuffers; }
		const std::shared_ptr<IndexBuffer>& getIndexBuffer() const { return m_indexBuffer; }

	private:
		unsigned int m_bufferID;

		std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
	};
}