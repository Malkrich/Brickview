#pragma once

namespace Brickview
{

	struct ShaderStorageBufferSpecifications
	{
		std::string BlockName = "<Unknown>";
		uint32_t BindingPoint = 0;
	};

	class ShaderStorageBuffer
	{
	public:
		static Ref<ShaderStorageBuffer> create(const ShaderStorageBufferSpecifications& specs, uint32_t size, const void* data);
		static Ref<ShaderStorageBuffer> create(const ShaderStorageBufferSpecifications& specs, uint32_t size);

		virtual ~ShaderStorageBuffer() = default;

		virtual uint32_t getSize() const = 0;

		virtual void resize(uint32_t size) = 0;
		virtual void setData(const void* data) = 0;
	};

}