#include "Pch.h"
#include "LDrawReader.h"

#include <charconv>

namespace Brickview
{
	namespace LDrawImpl
	{

		static LineType getLineType(const std::string& line)
		{
			if (line.empty())
				return LineType::Empty;

			size_t firstSpace = line.find_first_of(' ');
			size_t lineLength = line.size();
			std::string strPrefix = line.substr(0, firstSpace);
			std::string tail = line.substr(firstSpace + 1, lineLength - firstSpace);
			uint32_t lineType;
			std::from_chars(strPrefix.data(), strPrefix.data() + strPrefix.size(), lineType);

			return (LineType)lineType;
		}

	}

	LDrawTriangleData LDrawTriangleData::deserialize(const std::string& line)
	{
		BV_LOG_INFO("Deserializing triangle data: {}", line);
		LDrawTriangleData t;
		return t;
	}

	LDrawQuadData LDrawQuadData::deserialize(const std::string& line)
	{
		BV_LOG_INFO("Deserializing quad data: {}", line);
		LDrawQuadData q;
		return q;
	}

	LDrawSubFileRefData LDrawSubFileRefData::deserialize(const std::string& line)
	{
		BV_LOG_INFO("Deserializing sub-file reference data: {}", line);
		LDrawSubFileRefData sf;
		return sf;
	}

	LDrawReader::LDrawReader(const std::filesystem::path& filePath)
	{
		if (!std::filesystem::exists(filePath))
			m_valid = false;

		m_fileStream = std::ifstream(filePath);
	}

	std::string LDrawReader::lineTypeToString(LineType type)
	{
		switch (type)
		{
			case LineType::Empty:         return "Empty";
			case LineType::Comment:       return "Comment";
			case LineType::SubFileRef:    return "Sub-file reference";
			case LineType::Line:          return "Line";
			case LineType::Triangle:      return "Triangle";
			case LineType::Quadrilateral: return "Quadrilateral";
			case LineType::OptionalFile:  return "Optional file";
		}

		return "Unkown";
	}

	bool LDrawReader::readLine()
	{
		std::istream& s = std::getline(m_fileStream, m_currentLine);

		m_currentLineType = LDrawImpl::getLineType(m_currentLine);

		return (bool)s;
	}

}