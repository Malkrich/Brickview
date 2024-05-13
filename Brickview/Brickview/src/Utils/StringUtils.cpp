#include "Pch.h"
#include "StringUtils.h"

namespace Brickview::StringUtils
{

	size_t findNthCharacter(const std::string& data, char token, size_t nth)
	{
		size_t tokenCounter = 0;
		size_t startPos = 0;
		for (const char& c : data)
		{
			if (c == token)
				tokenCounter++;
			if (tokenCounter == nth)
				return startPos;
			startPos++;
		}
	}

}