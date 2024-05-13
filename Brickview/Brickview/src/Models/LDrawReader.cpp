#include "Pch.h"
#include "LDrawReader.h"
#include "Utils/StringUtils.h"

#include <glm/glm.hpp>

#include <filesystem>
#include <charconv>

namespace Brickview
{

	enum class LineType
	{
		Empty         = -1,
		Comment       = 0,
		SubFileRef    = 1,
		Line          = 2,
		Triangle      = 3,
		Quadrilateral = 4,
		OptionalFile  = 5
	};

	struct Triangle
	{
		static constexpr inline size_t getElementCount() { return 3; }
		static constexpr inline size_t getTriangleCount() { return 1; }

		std::array<glm::vec3, 3> Positions;
	};

	struct Quad
	{
		static constexpr inline size_t getElementCount() { return 4; }
		static constexpr inline size_t getTriangleCount() { return 2; }

		std::array<glm::vec3, 4> Positions;
	};

	namespace Utils
	{
		static inline glm::vec3 LDUToMM(const glm::vec3& v)
		{
			return v * 0.4f;
		}

		static LineType getPrefix(const std::string& line)
		{
			if (line.empty())
				return LineType::Empty;

			size_t firstSpace     = line.find_first_of(' ');
			std::string strPrefix = line.substr(0, firstSpace);
			uint32_t lineType;
			std::from_chars(strPrefix.data(), strPrefix.data() + strPrefix.size(), lineType);

			return (LineType)lineType;
		}

		static uint32_t getColorID(const std::string& line)
		{
			// ID colorID ...

			size_t firstSpace  = line.find_first_of(' ') + 1;
			size_t secondSpace = line.find_first_of(' ', firstSpace);
			std::string strColor = line.substr(firstSpace, secondSpace - firstSpace);
			uint32_t colorID;
			std::from_chars(strColor.data(), strColor.data() + strColor.size(), colorID);
			return colorID;
		}

		template<typename FaceType>
		static glm::vec3 computeNormal(const FaceType& triangle)
		{
			glm::vec3 u = glm::normalize(triangle.Positions[1] - triangle.Positions[0]);
			glm::vec3 v = glm::normalize(triangle.Positions[2] - triangle.Positions[0]);
			return glm::cross(u, v);
		}

		template<typename FaceType>
		static FaceType getFaceData(const std::string& line)
		{
			FaceType face;

			size_t startPos = StringUtils::findNthCharacter(line, ' ', 2);
			std::string lineData = line.substr(startPos + 1, line.size() - startPos + 1);
			std::stringstream ss(lineData);

			for (size_t i = 0; i < FaceType::getElementCount(); i++)
			{
				glm::vec3 pos;
				ss >> pos.x >> pos.y >> pos.z;
				face.Positions[i] = LDUToMM(pos);
			}

			return face;
		}

		template<typename FaceType>
		static void addFace(std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices, const std::string& line, uint32_t indexOffset)
		{
			FaceType faceData = Utils::getFaceData<FaceType>(line);
			glm::vec3 normal = Utils::computeNormal<FaceType>(faceData);

			// add new data
			vertices.reserve(vertices.size() + FaceType::getElementCount());
			for (const auto& position : faceData.Positions)
				vertices.push_back({ position, normal });

			indices.reserve(indices.size() + FaceType::getTriangleCount());
			uint32_t offset = 0;
			for (uint32_t i = 0; i < FaceType::getTriangleCount(); i++)
			{
				TriangleFace t = {
					(offset + 0) % FaceType::getElementCount(),
					(offset + 1) % FaceType::getElementCount(),
					(offset + 2) % FaceType::getElementCount()
				};
				t.addOffset(indexOffset);
				offset += 2;
				indices.push_back(t);
			}
		}

		// Utils function
		static std::string getComment(const std::string& line)
		{
			size_t firstSpace       = line.find_first_of(' ') + 1;
			firstSpace              = firstSpace == std::string::npos ? 0 : firstSpace;
			size_t commentLength    = line.size() - firstSpace;
			commentLength           = firstSpace == std::string::npos ? 0 : commentLength;
			return line.substr(firstSpace, commentLength);
		}
	}

	bool LDrawReader::load(const std::filesystem::path& filePath, std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices)
	{
		if (!std::filesystem::exists(filePath))
		{
			BV_LOG_ERROR("Couldn't load file: {}", filePath.generic_string());
			return false;
		}

		std::ifstream file(filePath);
		std::string line;
		std::stringstream stringStr;
		uint32_t indexOffset = 0;

		while (std::getline(file, line))
		{
			LineType lineType = Utils::getPrefix(line);
			uint32_t color;

			if (lineType == LineType::Empty)
				continue;

			if (lineType != LineType::Comment)
				color = Utils::getColorID(line);

			switch (lineType)
			{
				case LineType::Triangle:
				{
					Utils::addFace<Triangle>(vertices, indices, line, indexOffset);
					indexOffset += 3;
					break;
				}
				case LineType::Quadrilateral:
				{
					Utils::addFace<Quad>(vertices, indices, line, indexOffset);
					indexOffset += 4;
					break;
				}
			}
		}

		return true;
	}

}