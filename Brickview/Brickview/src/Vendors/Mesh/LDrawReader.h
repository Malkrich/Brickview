#pragma once

#include "LDrawCommandManager.h"

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

	struct LDrawCommandData
	{
		static LDrawCommandData deserialize(const std::string& line);

		LDrawCommandExtension Extension;
		std::vector<LDrawCommandArgument> Arguments;
	};

	class LDrawReader
	{
	public:
		LDrawReader(const std::filesystem::path& filePath);

		// For debug purposes
		static std::string lineTypeToString(LDrawLineType type);
		std::string getComment() const;

		bool readLine();

		bool isValid() const { return m_valid; }


		LDrawLineType getLineType() const { return m_currentLineType; }
		bool isCurrentLineCommand() const { return m_isCurrentLineCommand; }
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

		// Current states
		std::string m_currentLine;
		LDrawLineType m_currentLineType = LDrawLineType::Empty;
		bool m_isCurrentLineCommand = false;
	};

}