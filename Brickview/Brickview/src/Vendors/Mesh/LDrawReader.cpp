#include "Pch.h"
#include "LDrawReader.h"

#include "Utils/StringUtils.h"

#include <charconv>

namespace Brickview
{
	namespace Utils
	{

		static std::string deserializeElementAt(const std::string& line, uint32_t index)
		{
			return StringUtils::getSubStringAt(line, " ", index);
		}

		template<typename T>
		static T deserializePrimitiveTypeAt(const std::string& line, uint32_t index)
		{
			std::string subStr = deserializeElementAt(line, index);
			T value;
			std::from_chars(subStr.data(), subStr.data() + subStr.size(), value);
			return value;
		}

		static glm::vec3 deserializeVector3At(const std::string& line, uint32_t firstElementIndex)
		{
			glm::vec3 v(0.0);
			v.x = (float)deserializePrimitiveTypeAt<int32_t>(line, firstElementIndex + 0);
			v.y = (float)deserializePrimitiveTypeAt<int32_t>(line, firstElementIndex + 1);
			v.z = (float)deserializePrimitiveTypeAt<int32_t>(line, firstElementIndex + 2);
			return v;
		}

	}

	LDrawTriangleData LDrawTriangleData::deserialize(const std::string& line)
	{
		LDrawTriangleData t;
		t.p0 = Utils::deserializeVector3At(line, 2);
		t.p1 = Utils::deserializeVector3At(line, 5);
		t.p2 = Utils::deserializeVector3At(line, 8);
		return t;
	}

	LDrawQuadData LDrawQuadData::deserialize(const std::string& line)
	{
		LDrawQuadData q;
		q.p0 = Utils::deserializeVector3At(line, 2);
		q.p1 = Utils::deserializeVector3At(line, 5);
		q.p2 = Utils::deserializeVector3At(line, 8);
		q.p3 = Utils::deserializeVector3At(line, 11);
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

		m_currentLineType = deserializeLineType(m_currentLine);

		return (bool)s;
	}

	LineType LDrawReader::deserializeLineType(const std::string& line)
	{
		if (line.empty())
			return LineType::Empty;

		LineType lineType = (LineType)Utils::deserializePrimitiveTypeAt<uint32_t>(line, 0);
		return lineType;
	}

}