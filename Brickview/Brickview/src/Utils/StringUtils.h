#pragma once

#include <string>

namespace Brickview::StringUtils
{

		template<typename... Args>
		std::string format(const char* format, Args... args)
		{
			std::string returnString = "";
			sprintf(returnString.data(), format, std::forward<Args>(args)...);
			return returnString;
		}

		size_t findNthCharacter(const std::string& data, char token, size_t nth);

}