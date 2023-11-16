#pragma once

#define BIND_EVENT_FUNCTION(function) std::bind(&function, this, std::placeholders::_1)