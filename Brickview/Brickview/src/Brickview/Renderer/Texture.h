#pragma once

#include <cstdint>

namespace Brickview
{

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		virtual uint32_t getHandle() const = 0;

		template<typename T>
		T* as()
		{
			return reinterpret_cast<T*>(this);
		}
	};

}