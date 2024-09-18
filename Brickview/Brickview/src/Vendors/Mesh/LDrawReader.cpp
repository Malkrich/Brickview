#include "Pch.h"
#include "LDrawReader.h"

#include "Utils/StringUtils.h"

#include <glm/gtc/type_ptr.hpp>

#include <charconv>

namespace Brickview
{
	namespace Utils
	{

		static std::string deserializeElementAt(const std::string& line, uint32_t index)
		{
			return StringUtils::getSubStringAt(line, " ", index);
		}

		// For debug purposes
		static std::string deserializeComment(const std::string& line)
		{
			size_t beginIndex = StringUtils::findNthCharacter(line, ' ', 1);
			return line.substr(beginIndex+1);
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
			v.x = deserializePrimitiveTypeAt<float>(line, firstElementIndex + 0);
			v.y = deserializePrimitiveTypeAt<float>(line, firstElementIndex + 1);
			v.z = deserializePrimitiveTypeAt<float>(line, firstElementIndex + 2);
			return v;
		}

		static glm::mat4 deserializeTransform(const std::string& line, uint32_t index)
		{
			// from: https://www.ldraw.org/article/218.html
			float x, y, z, a, b, c, d, e, f, g, h, i;
			x = deserializePrimitiveTypeAt<float>(line, index + 0);
			y = deserializePrimitiveTypeAt<float>(line, index + 1);
			z = deserializePrimitiveTypeAt<float>(line, index + 2);
			a = deserializePrimitiveTypeAt<float>(line, index + 3);
			b = deserializePrimitiveTypeAt<float>(line, index + 4);
			c = deserializePrimitiveTypeAt<float>(line, index + 5);
			d = deserializePrimitiveTypeAt<float>(line, index + 6);
			e = deserializePrimitiveTypeAt<float>(line, index + 7);
			f = deserializePrimitiveTypeAt<float>(line, index + 8);
			g = deserializePrimitiveTypeAt<float>(line, index + 9);
			h = deserializePrimitiveTypeAt<float>(line, index + 10);
			i = deserializePrimitiveTypeAt<float>(line, index + 11);

			glm::mat4 transform;
			transform[0] = { a, d, g, 0.0f };
			transform[1] = { b, e, h, 0.0f };
			transform[2] = { c, f, i, 0.0f };
			transform[3] = { x, y, z, 1.0f };
			
			return transform;
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
		LDrawSubFileRefData sf;
		sf.Transform = Utils::deserializeTransform(line, 2);
		sf.FilePath  = Utils::deserializeElementAt(line, 14);
		return sf;
	}

	LDrawCommandData LDrawCommandData::deserialize(const std::string& line)
	{
		LDrawCommandData command;
		std::string rawCommandExtension = Utils::deserializeElementAt(line, 1);
		command.Extension = LDrawCommandManager::getCommandExtension(rawCommandExtension);

		uint32_t argIndex = 2;
		bool isCommandArgument = true;
		while (isCommandArgument)
		{
			std::string rawCommandArg = Utils::deserializeElementAt(line, argIndex);
			isCommandArgument = rawCommandArg != "";

			if (isCommandArgument)
			{
				LDrawCommandArgument arg = LDrawCommandManager::getCommandArgument(rawCommandArg);
				command.Arguments.push_back(arg);
			}

			argIndex++;
		}

		return command;
	}

	LDrawReader::LDrawReader(const std::filesystem::path& filePath)
	{
		if (!std::filesystem::exists(filePath))
			m_valid = false;

		m_fileStream = std::ifstream(filePath);
	}

	std::string LDrawReader::lineTypeToString(LDrawLineType type)
	{
		switch (type)
		{
			case LDrawLineType::Empty:         return "Empty";
			case LDrawLineType::Comment:       return "Comment";
			case LDrawLineType::SubFileRef:    return "Sub-file reference";
			case LDrawLineType::Line:          return "Line";
			case LDrawLineType::Triangle:      return "Triangle";
			case LDrawLineType::Quadrilateral: return "Quadrilateral";
			case LDrawLineType::OptionalFile:  return "Optional file";
		}

		return "Unkown";
	}

	std::string LDrawReader::getComment() const
	{
		std::string comment = Utils::deserializeComment(m_currentLine);
		return comment;
	}

	bool LDrawReader::readLine()
	{
		std::istream& s = std::getline(m_fileStream, m_currentLine);

		m_currentLineType = deserializeLineType(m_currentLine);

		if (m_currentLineType == LDrawLineType::Comment)
		{
			std::string rawCommandExtension = Utils::deserializeElementAt(m_currentLine, 1);
			m_isCurrentLineCommand = LDrawCommandManager::isCommand(rawCommandExtension);
		}
		else
			m_isCurrentLineCommand = false;

		return (bool)s;
	}

	LDrawLineType LDrawReader::deserializeLineType(const std::string& line)
	{
		if (line.empty())
			return LDrawLineType::Empty;

		LDrawLineType lineType = (LDrawLineType)Utils::deserializePrimitiveTypeAt<uint32_t>(line, 0);

		return lineType;
	}

}