#pragma once

#include "Core/Core.h"

#define BV_GPU_INTERNAL_PADDING(count) std::array<float, ##count> BV_CONCAT(__padding_, __LINE__)