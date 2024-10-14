#pragma once

#include "Renderer/Buffer.h"

namespace Brickview
{

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size, const void* data);
		OpenGLVertexBuffer(uint32_t size);

		virtual ~OpenGLVertexBuffer();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void setData(uint32_t size, void* data) override;

		virtual const Layout& getLayout() const override { return m_layout; }
		virtual void setBufferLayout(const Layout& layout) override { m_layout = layout; }

	private:
		uint32_t m_bufferID;
		Layout m_layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t size, const void* data);
		OpenGLIndexBuffer(uint32_t size);

		virtual ~OpenGLIndexBuffer();

		virtual void bind() const override;
		virtual void unbind() const override;
		virtual uint32_t getCount() const override { return m_count; }

		virtual void setData(uint32_t size, void* data) override;

	private:
		uint32_t m_bufferID;
		uint32_t m_count;
	};

	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();

		virtual ~OpenGLVertexArray();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void setIndexBuffer(const  Ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() override { return m_vertexBuffers; }
		virtual const Ref<IndexBuffer>& getIndexBuffer() const override { return m_indexBuffer; }

	private:
		uint32_t m_bufferID;

		std::vector<Ref<VertexBuffer>> m_vertexBuffers;
		Ref<IndexBuffer> m_indexBuffer;
	};

}