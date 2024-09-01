#include "Pch.h"
#include "LegoMeshLoader.h"

#include "Utils/StringUtils.h"
#include "LDrawReader.h"

#include <glm/glm.hpp>

namespace Brickview
{

	struct FileSpecifications
	{
		bool ClockwiseWinding = true;
		bool InvertNext = false;
	};

	struct SubMeshData
	{
		std::filesystem::path FilePath;
		glm::mat4 Transform;
		bool Inverted;
	};

	struct LDrawReaderData
	{
		std::filesystem::path BaseDirectory;
		std::filesystem::path SubPartsDirectory;
		std::filesystem::path PartsDirectory;

		using CommandFunction = std::function<void(const std::string&, FileSpecifications&)>;
		std::unordered_map<std::string, CommandFunction> CommandFunctions;
	};

	static LDrawReaderData* s_ldrawData = nullptr;

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

		static std::tuple<uint32_t, std::string> getColorID(const std::string& line)
		{
			// colorID ...

			size_t strColorLength = line.find_first_of(' ');
			size_t lineLength     = line.size();
			std::string strColor  = line.substr(0, strColorLength);
			uint32_t colorID;
			std::from_chars(strColor.data(), strColor.data() + strColor.size(), colorID);

			std::string coordData = line.substr(strColorLength + 1, lineLength - strColorLength);

			return { colorID, coordData };
		}

		template<typename FaceType>
		static void addFace(const glm::mat4& transform, bool inverted, const FileSpecifications& spec,
			std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices,
			const std::string& coordData,
			uint32_t indexOffset)
		{
			std::stringstream ss(coordData);

			FaceType face;
			for (size_t i = 0; i < FaceType::getElementCount(); i++)
			{
				glm::vec3 pos;
				ss >> pos.x >> pos.y >> pos.z;
				//pos.y *= -1.0f;
				face.Positions[i] = transform * glm::vec4(pos, 1.0f);
			}

			glm::vec3 u = face.Positions[1] - face.Positions[0];
			glm::vec3 v = face.Positions[2] - face.Positions[0];
			glm::vec3 normal = glm::normalize(glm::cross(u, v));
			if ((!spec.ClockwiseWinding && !inverted) || (spec.ClockwiseWinding && inverted))
				normal *= -1.0f; // Inverse normal

			// Vertices
			vertices.reserve(vertices.size() + FaceType::getElementCount());
			for (const auto& position : face.Positions)
				vertices.push_back({ position, normal });

			// Indices
			indices.reserve(indices.size() + FaceType::getTriangleCount());

			TriangleFace t = { 0, 1, 2 };
			t.addOffset(indexOffset);
			indices.push_back(t);

			if (FaceType::getTriangleCount() == 1)
				return;

			t = { 2, 3, 0 };
			t.addOffset(indexOffset);
			indices.push_back(t);
		}

		static SubMeshData readSubMesh(const std::string& line)
		{
			// find file name
			size_t lastSpace      = line.rfind(' ');
			size_t fileNameLength = line.size() - lastSpace;
			std::string fileName  = line.substr(lastSpace + 1, fileNameLength);

			// find transform
			std::string transformData = line.substr(0, lastSpace);
			std::stringstream ss(transformData);

			float a, b, c, d, e, f, g, h, i, x, y, z;
			ss  >> x >> y >> z
				>> a >> b >> c
				>> d >> e >> f
				>> g >> h >> i;

			SubMeshData subMesh;
			subMesh.FilePath = fileName;
			subMesh.Transform = glm::mat4(
				a,  d,  g,  0.0f,
				b,  e,  h,  0.0f,
				c,  f,  i,  0.0f,
				x,  y,  z,  1.0f);

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

		static void readBFCCommand(const std::string& commandContent, FileSpecifications& spec)
		{
			std::stringstream ss(commandContent);
			while (ss.good())
			{
				std::string instruction;
				ss >> instruction;

				if (instruction == "CW")
					spec.ClockwiseWinding = true;
				else if (instruction == "CCW")
					spec.ClockwiseWinding = false;
				else if (instruction == "INVERTNEXT")
					spec.InvertNext = true;
				else
					continue;
			}
		}

		static std::tuple<std::string, std::string> getCommandPrefix(const std::string& line)
		{
			// Word

			size_t prefixLength  = line.find_first_of(' ');
			prefixLength         = prefixLength == std::string::npos ? line.size() : prefixLength;
			size_t contentLength = line.size();

			std::string commandPrefix  = line.substr(0, prefixLength);
			std::string commandContent = contentLength == prefixLength ? "" : line.substr(prefixLength + 1, contentLength - prefixLength);

			return { commandPrefix, commandContent };
		}

	}

	void LegoMeshLoader::init()
	{
		BV_ASSERT(!s_ldrawData, "LDrawReader engine already initialized!");

		s_ldrawData                    = new LDrawReaderData();
		s_ldrawData->BaseDirectory     = "./data/Models/LDrawExample";
		s_ldrawData->SubPartsDirectory = s_ldrawData->BaseDirectory / "p";
		s_ldrawData->PartsDirectory    = s_ldrawData->BaseDirectory / "parts";

		s_ldrawData->CommandFunctions["BFC"] = &Utils::readBFCCommand;
	}

	void LegoMeshLoader::shutdown()
	{
		delete s_ldrawData;
		s_ldrawData = nullptr;
	}

	bool LegoMeshLoader::load(const std::filesystem::path& filePath, Ref<Mesh> mesh)
	{
		std::vector<Vertex> vertices;
		std::vector<TriangleFace> indices;

		if (!std::filesystem::exists(filePath))
		{
			BV_LOG_ERROR("Couldn't load file: {}", filePath.generic_string());
			return false;
		}

		std::queue<SubMeshData> subMeshes;
		SubMeshData initialMesh = { filePath, glm::mat4(1.0f) };
		subMeshes.push(initialMesh);

		do
		{
			const auto& meshData = subMeshes.front();
			readFile(meshData, mesh, subMeshes);
			subMeshes.pop();
		} while (!subMeshes.empty());

		for (auto& vertex : vertices)
			vertex.Position = Utils::LDUToM(vertex.Position);

		return true;
	}

	bool LegoMeshLoader::readFile(const SubMeshData& meshData, Ref<Mesh> mesh, std::queue<SubMeshData>& subMeshes)
	{
		std::filesystem::path filePath = meshData.FilePath;

		LDrawReader reader(filePath);

		if (!reader.isValid())
		{
			BV_LOG_ERROR("Couldn't load file: {}", filePath.generic_string());
			return false;
		}

		while (reader.readLine())
		{
			LineType lineType = reader.getLineType();

			switch (lineType)
			{
				case LineType::Empty:
					continue;
			}
		}

		return true;
#if 0
		std::filesystem::path filePath = meshData.FilePath;
		glm::mat4 transform = meshData.Transform;
		bool inverted = meshData.Inverted;

		if (!std::filesystem::exists(filePath))
		{
			BV_LOG_ERROR("Couldn't load file: {}", filePath.generic_string());
			return false;
		}

		FileSpecifications spec;

		std::filesystem::path currentDirectory = filePath.parent_path();
		std::ifstream file(filePath);
		std::string line;
		std::stringstream stringStr;
		uint32_t indexOffset = vertices.size();

		while (std::getline(file, line))
		{
			auto [lineType, lineContent] = Utils::getLineType(line);

			if (lineType == LineType::Empty || lineType == LineType::Line) // TODO: Implement lines
				continue;

			if (lineType == LineType::Comment)
			{
				auto [prefix, commandContent] = Utils::getCommandPrefix(lineContent);

				if (!isPrefixACommand(prefix))
					continue;

				// Call command
				s_ldrawData->CommandFunctions.at(prefix)(commandContent, spec);
				continue;
			}

			auto [colorID, geoData] = Utils::getColorID(lineContent);
			switch (lineType)
			{
			case LineType::Triangle:
			{
				Utils::addFace<Triangle>(transform, inverted, spec, vertices, indices, geoData, indexOffset);
				indexOffset += 3;
				break;
			}
			case LineType::Quadrilateral:
			{
				Utils::addFace<Quad>(transform, inverted, spec, vertices, indices, geoData, indexOffset);
				indexOffset += 4;
				break;
			}
			case LineType::SubFileRef:
			{
				SubMeshData subMesh = Utils::readSubMesh(geoData);

				if (isInPartsDirectory(subMesh.FilePath))
				{
					BV_LOG_INFO("Found file {} in {}", subMesh.FilePath.generic_string(), s_ldrawData->PartsDirectory.generic_string());
					subMesh.FilePath = s_ldrawData->PartsDirectory / subMesh.FilePath;
				}
				else if (isInSubPartsDirectory(subMesh.FilePath))
				{
					BV_LOG_INFO("Found file {} in {}", subMesh.FilePath.generic_string(), s_ldrawData->SubPartsDirectory.generic_string());
					subMesh.FilePath = s_ldrawData->SubPartsDirectory / subMesh.FilePath;
				}
				else
					BV_LOG_ERROR("Sub mesh {} doesn't exist!", subMesh.FilePath.generic_string());

				glm::mat4 worldTransform = transform * subMesh.Transform;
				subMesh.Transform = worldTransform;
				subMesh.Inverted = spec.InvertNext;
				if (spec.InvertNext)
					spec.InvertNext = false;

				subMeshes.push(subMesh);

				BV_LOG_INFO("Added {} to the mesh queue", subMesh.FilePath.generic_string());
				break;
			}
			}
		}

		return true;
#endif
	}

	bool LegoMeshLoader::isInPartsDirectory(const std::filesystem::path& filePath)
	{
		return std::filesystem::exists(s_ldrawData->PartsDirectory / filePath);
	}

	bool LegoMeshLoader::isInSubPartsDirectory(const std::filesystem::path& filePath)
	{
		return std::filesystem::exists(s_ldrawData->SubPartsDirectory / filePath);
	}

	bool LegoMeshLoader::isPrefixACommand(const std::string& prefix)
	{
		return s_ldrawData->CommandFunctions.find(prefix) != s_ldrawData->CommandFunctions.end();
	}

}