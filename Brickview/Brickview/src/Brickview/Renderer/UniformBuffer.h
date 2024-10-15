#pragma once

namespace Brickview
{

	// TODO: handle arrays
	enum class UniformBufferElementType
	{
		None = 0,
		Bool,
		Float, Float2, Float3,
		Int, Int2, Int3,
		Mat2, Mat3, Mat4,
		// Array
	};

	struct UniformBufferElement
	{
		UniformBufferElementType Type = UniformBufferElementType::None;
		uint32_t Offset = 0; // Calculated by the uniform buffer layout class
		uint32_t Size = 0; // Calculated by the uniform buffer layout class
		//uint32_t ElementCount = 1; // Potentialy for arrays

		UniformBufferElement(UniformBufferElementType type)
			: Type(type)
		{}
	};

	class UniformBufferLayout
	{
	public:
		UniformBufferLayout() = default;
		UniformBufferLayout(std::initializer_list<UniformBufferElement> elements);

		uint32_t getBufferSize() const { return m_bufferSize; }

		uint32_t getElementCount() const { return m_elements.size(); }

		const UniformBufferElement& getBufferElement(uint32_t index) const
		{
			BV_ASSERT(index < m_elements.size(), "Index {} is out of bound!", index);
			return m_elements[index];
		}

	private:
		void calculateSizeAndElementOffsets();

	private:
		std::vector<UniformBufferElement> m_elements = {};
		uint32_t m_bufferSize = 0;
	};

	struct UniformBufferSpecifications
	{
		uint32_t BindingPoint = 0;
		UniformBufferLayout Layout;

		UniformBufferSpecifications() = default;
	};

	class UniformBuffer
	{
	public:
		static Ref<UniformBuffer> create(const UniformBufferSpecifications& specs);

		virtual ~UniformBuffer() = default;

		virtual void setElement(uint32_t elementIndex, const void* data) = 0;

		virtual const UniformBufferSpecifications& getSpecifications() const = 0;
	};

}