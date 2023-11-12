#include "Log.h"

#include <spdlog\sinks\stdout_color_sinks.h>
#include <spdlog\common.h>

namespace Brickview
{
	std::shared_ptr<spdlog::logger> Log::s_logger;

	void Log::init()
	{
		s_logger = spdlog::stdout_color_mt("Brickview");
		spdlog::set_level(spdlog::level::trace);
		s_logger->set_pattern("%^[%T] %n %l : %v%$");
	}
}