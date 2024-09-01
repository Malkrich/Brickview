#pragma once

#include <filesystem>

namespace Brickview
{

	enum class LineType
	{
		Empty = -1,
		Comment = 0,
		SubFileRef = 1,
		Line = 2,
		Triangle = 3,
		Quadrilateral = 4,
		OptionalFile = 5
	};

	class LDrawReader
	{
	public:
		LDrawReader(const std::filesystem::path& filePath);

		bool isValid() const { return m_valid; }
		LineType getLineType() const { return m_currentLineType; }

		bool readLine();

	private:
		// Global file states
		std::ifstream m_fileStream;
		bool m_valid = true;

		// Current states
		std::string m_currentLine;
		LineType m_currentLineType;

	};

}