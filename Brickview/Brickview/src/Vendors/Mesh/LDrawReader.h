#pragma once

#include <glm/glm.hpp>

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

	struct LDrawTriangleData
	{
		static LDrawTriangleData deserialize(const std::string& line);

		glm::vec3 p0, p1, p2;
	};

	struct LDrawQuadData
	{
		static LDrawQuadData deserialize(const std::string& line);

		glm::vec3 p0, p1, p2, p3;
	};

	struct LDrawSubFileRefData
	{
		static LDrawSubFileRefData deserialize(const std::string& line);

		std::filesystem::path FilePath;
		glm::mat4 Transform;
	};

	class LDrawReader
	{
	public:
		LDrawReader(const std::filesystem::path& filePath);

		// For debug purposes
		static std::string lineTypeToString(LineType type);

		bool readLine();

		bool isValid() const { return m_valid; }

		LineType getLineType() const { return m_currentLineType; }
		template<typename T>
		T getLineData() const
		{
			T data = T::deserialize(m_currentLine);
			return data;
		}

	private:
		LineType deserializeLineType(const std::string& line);

	private:
		// Global file states
		std::ifstream m_fileStream;
		bool m_valid = true;

		// Current states
		std::string m_currentLine;
		LineType m_currentLineType = LineType::Empty;
	};

}