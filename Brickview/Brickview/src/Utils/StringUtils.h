#pragma once

#include <string>

namespace Brickview
{

	class StringUtils
	{
	public:
		template<typename... Args>
		static std::string format(const char* format, Args... args)
		{
			std::string returnString = "";
			sprintf(returnString.data(), format, std::forward<Args>(args)...);
			return returnString;
		}
	};

}