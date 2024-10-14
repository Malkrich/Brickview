#include "Pch.h"
#include "Layout.h"

namespace Utils
{
	using BufferElementType = Brickview::BufferElementType;

	static uint32_t getElementSize(BufferElementType type)
	{
		switch (type)
		{
			case BufferElementType::Bool:	return 1;
			case BufferElementType::Int:	return 1 * 4;
			case BufferElementType::Int2:	return 2 * 4;
			case BufferElementType::Int3:	return 3 * 4;
			case BufferElementType::Int4:	return 4 * 4;
			case BufferElementType::Float:	return 1 * 4;
			case BufferElementType::Float2: return 2 * 4;
			case BufferElementType::Float3: return 3 * 4;
			case BufferElementType::Float4: return 4 * 4;
			case BufferElementType::Mat2:	return 2 * 2 * 4;
			case BufferElementType::Mat3:	return 3 * 3 * 4;
			case BufferElementType::Mat4:	return 4 * 4 * 4;
		}

		BV_ASSERT(false, "Buffer element type unknown !");
		return 0;
	}
}

namespace Brickview
{

	Layout::Layout(const std::initializer_list<BufferElement>& elements)
		: m_elements(elements)
	{
		computeOffsetAndStride();
	}

	void Layout::computeOffsetAndStride()
	{
		uint32_t offset = 0;
		uint32_t index = m_elements[0].Index;
		for(auto& element : m_elements)
		{
			element.Index = index;
			element.Offset = offset;
			uint32_t elementSize = Utils::getElementSize(element.Type);
			m_stride += elementSize;
			offset += elementSize;
			index++;
		}
	}

}