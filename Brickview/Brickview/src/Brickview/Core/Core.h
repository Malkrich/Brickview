#pragma once

#define BV_BIND_EVENT_FUNCTION(function) std::bind(&function, this, std::placeholders::_1)

#ifdef BV_DEBUG
	#define BV_ASSERT(condition, ...) if(!(condition)) { BV_LOG_ERROR(__VA_ARGS__); __debugbreak(); }
#else
	#define BV_ASSERT(condition, ...)
#endif

#include <memory>

namespace Brickview
{

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename... Args>
	constexpr Scope<T> createScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename... Args>
	constexpr Ref<T> createRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	// std add-ons
	// from boost: https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
	template<typename T>
	inline void hashCombine(size_t& seed, const T& v)
	{
		std::hash<T> hv;
		seed ^= hv(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

}