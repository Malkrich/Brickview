#pragma once

// Prevents warnings from the external header
#pragma warning(push, 0)
	#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace Brickview
{

	class Log
	{
	public:
		Log() = default;

		static void init();

		static std::shared_ptr<spdlog::logger> getLogger() { return s_logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_logger;
	};

}

#ifdef BV_DEBUG
	#define BV_LOG_TRACE(...)	Brickview::Log::getLogger()->trace(__VA_ARGS__)
	#define BV_LOG_INFO(...)	Brickview::Log::getLogger()->info(__VA_ARGS__)
	#define BV_LOG_WARN(...)	Brickview::Log::getLogger()->warn(__VA_ARGS__)
	#define BV_LOG_ERROR(...)	Brickview::Log::getLogger()->error(__VA_ARGS__)
#else
	#define BV_LOG_TRACE(...)
	#define BV_LOG_INFO(...)
	#define BV_LOG_WARN(...)
	#define BV_LOG_ERROR(...)
#endif