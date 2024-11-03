#pragma once

namespace Brickview
{

	// TODO: handle arrays
	enum class UniformBufferElementType
	{
		None = 0,
		Bool,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Mat2, Mat3, Mat4,
	};

	struct UniformBufferElement
	{
		std::vector<UniformBufferElementType> Types = {};
		uint32_t Offset = 0;   // Calculated by the uniform buffer layout class
		uint32_t Size = 0;     // Calculated by the uniform buffer layout class
		uint32_t ElementCount; // Potentialy for arrays

		UniformBufferElement(UniformBufferElementType type, uint32_t arrayElementCount = 1)
			: ElementCount(arrayElementCount) 
		{
			Types.push_back(type);
		}
		UniformBufferElement(const std::initializer_list<UniformBufferElementType>& structLayoutTypes, uint32_t arrayElementCount = 1)
			: Types(structLayoutTypes)
			, ElementCount(arrayElementCount) {}
	};

	class UniformBufferLayout
	{
	public:
		UniformBufferLayout() = default;
		UniformBufferLayout(const std::initializer_list<UniformBufferElement>& elements);

		uint32_t getBufferSize() const { return m_bufferSize; }

		uint32_t getElementCount() const { return m_elements.size(); }

		const UniformBufferElement& getBufferElement(uint32_t index) const
		{
			BV_ASSERT(index < m_elements.size(), "Index {} is out of bound!", index);
			return m_elements[index];
		}

	private:
		void calculateElementSizesAndOffsets();

	private:
		std::vector<UniformBufferElement> m_elements = {};
		uint32_t m_bufferSize = 0;
	};

	struct UniformBufferSpecifications
	{
		std::string BlockName = "<Unknown>";
		uint32_t BindingPoint = 0;
		UniformBufferLayout Layout;

		UniformBufferSpecifications() = default;
	};

	class UniformBuffer
	{
	public:
		static Ref<UniformBuffer> create(const UniformBufferSpecifications& specs);

		virtual ~UniformBuffer() = default;

		virtual void setElements(const void* data) = 0;
		virtual void setElement(uint32_t elementIndex, const void* data) = 0;

		virtual const UniformBufferSpecifications& getSpecifications() const = 0;
	};

}