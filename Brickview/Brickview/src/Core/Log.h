#pragma once

#include <spdlog/spdlog.h>

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

	#define BV_ASSERT(condition, ...) if(!condition) { BV_LOG_ERROR(__VA_ARGS__); }
#else
	#define BV_LOG_TRACE(...)
	#define BV_LOG_INFO(...)
	#define BV_LOG_WARN(...)
	#define BV_LOG_ERROR(...)

	#define BV_ASSERT(condition, ...)
#endif