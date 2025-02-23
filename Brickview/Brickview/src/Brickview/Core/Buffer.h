#pragma once

namespace Brickview
{

	struct Buffer
	{
		uint32_t Size = 0;
		uint8_t* Data = nullptr;

		Buffer() = default;
		Buffer(const Buffer&) = default;
		Buffer(uint32_t size, const uint8_t* data = nullptr);
		~Buffer();

		static Buffer copy(const Buffer& other);

		void release();
		void allocate(uint32_t size);

		operator bool() const { return Data != nullptr; }
	};

}