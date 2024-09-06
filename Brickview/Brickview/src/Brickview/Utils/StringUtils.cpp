#include "Pch.h"
#include "StringUtils.h"

namespace Brickview::StringUtils
{

	size_t findNthCharacter(const std::string& data, char token, size_t nth)
	{
		size_t charPos = 0;
		size_t offset = 0;

		for (uint32_t i = 0; i < nth; i++)
		{
			charPos = data.find_first_of(token, offset);

			if (charPos == std::string::npos)
				break;

			offset = charPos + 1;
		}
		return charPos;
	}

	std::string getSubStringAt(const std::string& data, const char* separator, uint32_t index)
	{
		size_t subStrBegin = 0;

		for (uint32_t i = 0; i < index; i++)
		{
			size_t firstSep = data.find_first_of(separator, subStrBegin);

			if (firstSep == std::string::npos)
				return "";

			subStrBegin = firstSep + 1;
		}

		size_t subStrSize = data.find_first_of(separator, subStrBegin) - subStrBegin;
		return data.substr(subStrBegin, subStrSize);
	}

}