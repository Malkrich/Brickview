#pragma once

#include "Log.h"

#define BV_BIND_EVENT_FUNCTION(function) std::bind(&function, this, std::placeholders::_1)

#ifdef BV_DEBUG
	#define BV_ASSERT(condition, ...) if(!(condition)) { BV_LOG_ERROR(__VA_ARGS__); __debugbreak(); }
#else
	#define BV_ASSERT(condition, ...)
#endif