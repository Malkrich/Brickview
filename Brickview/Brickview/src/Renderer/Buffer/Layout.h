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
		BufferElement(const std::string& name, BufferElementType type, bool normalized = false)
			: Name(name)
			, Type(type)
			, Normalized(normalized)
		{}

		std::string Name;
		unsigned int Index = 0;
		BufferElementType Type;
		bool Normalized;
		unsigned int Offset = 0;
	};

	class Layout
	{
	public:
		Layout() = default;
		Layout(const std::initializer_list<BufferElement>& elements);

		unsigned int getStride() const { return m_stride; }

		std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }

	private:
		void computeOffsetAndStride();

	private:
		unsigned int m_stride = 0;
		std::vector<BufferElement> m_elements;
	};

}