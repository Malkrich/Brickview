#include "Pch.h"
#include "LDrawReader.h"
#include "Utils/StringUtils.h"

#include <glm/glm.hpp>

#include <charconv>

namespace Brickview
{

	struct LDrawReaderData
	{
		std::filesystem::path BaseDirectory;
		std::filesystem::path SubPartsDirectory;
		std::filesystem::path PartsDirectory;
	};

	static LDrawReaderData* s_ldrawData = nullptr;

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

	template<uint32_t ElementCount>
	struct BasicFace
	{
		std::array<glm::vec3, ElementCount> Positions;
	};

	struct Triangle : public BasicFace<3>
	{
		static constexpr inline size_t getElementCount() { return 3; }
		static constexpr inline size_t getTriangleCount() { return 1; }
	};

	struct Quad : public BasicFace<4>
	{
		static constexpr inline size_t getElementCount() { return 4; }
		static constexpr inline size_t getTriangleCount() { return 2; }
	};

	struct SubMeshData
	{
		std::filesystem::path FilePath;
		glm::mat4 Transform;
	};

	namespace Utils
	{
		template<typename T>
		static inline glm::vec3 LDUToMM(const T& v)
		{
			return v * 0.4f;
		}

		template<typename T>
		static inline glm::vec3 LDUToM(const T& v)
		{
			return LDUToMM<T>(v) / 1000.0f;
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
		static void addFace(const glm::mat4& transform, std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices, const std::string& line, uint32_t indexOffset)
		{
			size_t startPos = StringUtils::findNthCharacter(line, ' ', 2);
			std::string lineData = line.substr(startPos + 1, line.size() - startPos + 1);
			std::stringstream ss(lineData);

			FaceType face;
			for (size_t i = 0; i < FaceType::getElementCount(); i++)
			{
				glm::vec3 pos;
				ss >> pos.x >> pos.y >> pos.z;
				face.Positions[i] = transform * glm::vec4(pos, 1.0f);
			}

			glm::vec3 u = face.Positions[1] - face.Positions[0];
			glm::vec3 v = face.Positions[2] - face.Positions[0];
			glm::vec3 normal = glm::normalize(glm::cross(v, u));

			// Vertices
			vertices.reserve(vertices.size() + FaceType::getElementCount());
			for (const auto& position : face.Positions)
				vertices.push_back({ position, normal });

			// Indices
			indices.reserve(indices.size() + FaceType::getTriangleCount());
			switch (FaceType::getTriangleCount())
			{
				case 1:
				{
					TriangleFace t = { 0, 1, 2 };
					t.addOffset(indexOffset);
					indices.push_back(t);
					return;
				}
				case 2:
				{
					TriangleFace t1 = { 0, 1, 2 };
					TriangleFace t2 = { 2, 3, 0 };
					t1.addOffset(indexOffset);
					t2.addOffset(indexOffset);
					indices.push_back(t1);
					indices.push_back(t2);
					return;
				}
			}
		}

		static SubMeshData readSubMesh(const std::string& line)
		{
			// find file name
			size_t lastSpace      = line.rfind(' ');
			size_t fileNameLength = line.size() - lastSpace;
			std::string fileName  = line.substr(lastSpace + 1, fileNameLength);

			// find transform
			size_t secondSpace = StringUtils::findNthCharacter(line, ' ', 2);
			size_t transformDataLength = lastSpace - secondSpace;
			std::string transformData = line.substr(secondSpace + 1, transformDataLength);
			std::stringstream ss(transformData);

			float a, b, c, d, e, f, g, h, i, x, y, z;
			ss  >> x >> y >> z
				>> a >> b >> c
				>> d >> e >> f
				>> g >> h >> i;

			SubMeshData subMesh;
			subMesh.FilePath = fileName;
			subMesh.Transform = glm::mat4(
				a, d, g, 0.0f,
				b, e, h, 0.0f,
				c, f, i, 0.0f,
				x, y, z, 1.0f);

			return subMesh;
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

		bool isInPartsDirectory(const std::filesystem::path& filePath)
		{
			return std::filesystem::exists(s_ldrawData->PartsDirectory / filePath);
		}

		bool isInSubPartsDirectory(const std::filesystem::path& filePath)
		{
			return std::filesystem::exists(s_ldrawData->SubPartsDirectory / filePath);
		}
	}

	void LDrawReader::init()
	{
		BV_ASSERT(!s_ldrawData, "LDrawReader engine already initialized!");

		s_ldrawData                    = new LDrawReaderData();
		s_ldrawData->BaseDirectory     = "./data/LDraw";
		s_ldrawData->SubPartsDirectory = s_ldrawData->BaseDirectory / "p";
		s_ldrawData->PartsDirectory    = s_ldrawData->BaseDirectory / "parts";
	}

	void LDrawReader::shutdown()
	{
		delete s_ldrawData;
		s_ldrawData = nullptr;
	}

	bool LDrawReader::load(const std::filesystem::path& filePath, std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices)
	{
		if (!std::filesystem::exists(filePath))
		{
			BV_LOG_ERROR("Couldn't load file: {}", filePath.generic_string());
			return false;
		}

		std::queue<SubMeshData> subMeshes;
		SubMeshData initialMesh = { filePath, glm::mat4(1.0f) };
		subMeshes.push(initialMesh);

		uint32_t meshCount = 0;
		do
		{
			const auto& mesh = subMeshes.front();
			readFile(mesh.FilePath, mesh.Transform, vertices, indices, subMeshes);
			meshCount++;
			subMeshes.pop();
		} while (!subMeshes.empty());
		BV_LOG_INFO("Loaded {} meshes.", meshCount);

		for (auto& vertex : vertices)
			vertex.Position = Utils::LDUToMM(vertex.Position);

		return true;
	}

	bool LDrawReader::readFile(const std::filesystem::path& filePath, const glm::mat4& transform,
		std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices,
		std::queue<SubMeshData>& subMeshes)
	{
		if (!std::filesystem::exists(filePath))
		{
			BV_LOG_ERROR("Couldn't load file: {}", filePath.generic_string());
			return false;
		}

		std::filesystem::path currentDirectory = filePath.parent_path();
		std::ifstream file(filePath);
		std::string line;
		std::stringstream stringStr;
		uint32_t indexOffset = vertices.size();

		while (std::getline(file, line))
		{
			LineType lineType = Utils::getPrefix(line);
//			uint32_t color;

			if (lineType == LineType::Empty || lineType == LineType::Comment)
				continue;

//			color = Utils::getColorID(line);

			switch (lineType)
			{
				case LineType::Triangle:
				{
					Utils::addFace<Triangle>(transform, vertices, indices, line, indexOffset);
					indexOffset += 3;
					break;
				}
				case LineType::Quadrilateral:
				{
					Utils::addFace<Quad>(transform, vertices, indices, line, indexOffset);
					indexOffset += 4;
					break;
				}
				case LineType::SubFileRef:
				{
					SubMeshData subMesh = Utils::readSubMesh(line);
					
					if (Utils::isInPartsDirectory(subMesh.FilePath))
					{
						BV_LOG_INFO("Found file {} in {}", subMesh.FilePath.generic_string(), s_ldrawData->PartsDirectory.generic_string());
						subMesh.FilePath = s_ldrawData->PartsDirectory / subMesh.FilePath;
					}
					else if (Utils::isInSubPartsDirectory(subMesh.FilePath))
					{
						BV_LOG_INFO("Found file {} in {}", subMesh.FilePath.generic_string(), s_ldrawData->SubPartsDirectory.generic_string());
						subMesh.FilePath = s_ldrawData->SubPartsDirectory / subMesh.FilePath;
					}
					else
						BV_LOG_ERROR("Sub nesh {} doen't exist!", subMesh.FilePath.generic_string());

					subMesh.Transform *= transform;

					subMeshes.push(subMesh);

					BV_LOG_INFO("Added {} to the mesh queue", subMesh.FilePath.generic_string());
					break;
				}
			}
		}

		return true;
	}

}