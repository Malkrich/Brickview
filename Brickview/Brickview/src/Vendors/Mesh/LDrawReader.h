#pragma once

#include <glm/glm.hpp>

#include <filesystem>

namespace Brickview
{

	enum class LDrawLineType
	{
		Empty = -1,
		Comment = 0,
		SubFileRef = 1,
		Line = 2,
		Triangle = 3,
		Quadrilateral = 4,
		OptionalFile = 5,
	};

	enum class LDrawGeometryWinding { Unknown = 0, CW, CCW };

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

	enum class LDrawCommandExtension;
	enum class LDrawCommandArgument;
	struct LDrawCommandData
	{
		static LDrawCommandData deserialize(const std::string& line);

		LDrawCommandExtension Extension;
		std::vector<LDrawCommandArgument> Arguments;
	};

	class LDrawReader
	{
	public:
		struct States
		{
			LDrawGeometryWinding Winding = LDrawGeometryWinding::Unknown;
			bool Inverted = false;
		};

	public:
		LDrawReader(const std::filesystem::path& filePath, bool inverted);

		// For debug purposes
		static std::string lineTypeToString(LDrawLineType type);
		std::string getComment() const;

		bool readLine();
		bool isValid() const { return m_valid; }

		LDrawLineType getLineType() const { return m_currentLineType; }
		LDrawGeometryWinding getCurrentWindingState() const;
		bool isCurrentLineInverted() const { return m_currentLineStates.Inverted; }
		template<typename T>
		T getLineData() const
		{
			T data = T::deserialize(m_currentLine);
			return data;
		}

	private:
		LDrawLineType deserializeLineType(const std::string& line);

	private:
		// Global file states
		std::ifstream m_fileStream;
		bool m_valid = true;
		bool m_fileInverted;

		// Current states
		std::string m_currentLine;
		LDrawLineType m_currentLineType = LDrawLineType::Empty;
		States m_currentLineStates;
	};

}