#pragma once

namespace Brickview
{
	enum class BufferElementType
	{
		Bool = 0,
		Int, Int2, Int3, Int4,
		Float, Float2, Float3, Float4,
		Mat2, Mat3, Mat4
	};

	struct BufferElement
	{
		BufferElement(uint32_t index, const std::string& name, BufferElementType type)
			: Name(name)
			, Index(index)
			, Type(type)
		{}
		BufferElement(uint32_t index, const std::string& name, BufferElementType type, uint32_t attributeDivisor)
			: Name(name)
			, Index(index)
			, Type(type)
			, AttributeDivisor(attributeDivisor)
		{}

		std::string Name;
		uint32_t Index;
		BufferElementType Type;
		uint32_t AttributeDivisor = 0;
		bool Normalized = false;
		uint32_t Offset = 0;
	};

	class Layout
	{
	public:
		Layout() = default;
		Layout(const std::initializer_list<BufferElement>& elements);

		uint32_t getStride() const { return m_stride; }

		std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }

	private:
		void computeOffsetAndStride();

	private:
		uint32_t m_stride = 0;
		std::vector<BufferElement> m_elements;
	};

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