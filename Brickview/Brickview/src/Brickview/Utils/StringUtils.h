#pragma once

#include <string>

namespace Brickview::StringUtils
{

		size_t findNthCharacter(const std::string& data, char token, size_t nth);

		std::string getSubStringAt(const std::string& data, const char* separator, uint32_t index);
		
}