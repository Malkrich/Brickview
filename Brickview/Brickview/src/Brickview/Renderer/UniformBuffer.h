#pragma once

namespace Brickview
{
	struct UniformBufferSpecifications
	{
		std::string BlockName = "<Unknown>";
		uint32_t BindingPoint = 0;

		UniformBufferSpecifications() = default;
	};

	class UniformBuffer
	{
	public:
		static Ref<UniformBuffer> create(const UniformBufferSpecifications& specs, uint32_t size);
		static Ref<UniformBuffer> create(const UniformBufferSpecifications& specs, uint32_t size, const void* data);

		virtual ~UniformBuffer() = default;

		virtual void setData(const void* data) = 0;
		virtual void resize(uint32_t newSize) = 0;

		virtual const UniformBufferSpecifications& getSpecifications() const = 0;
	};

}