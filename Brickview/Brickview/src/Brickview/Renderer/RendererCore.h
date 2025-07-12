#pragma once

#include "Core/Core.h"

#define BV_GPU_INTERNAL_PADDING(count) float BV_CONCAT(__padding_, __LINE__)[##count] = {}