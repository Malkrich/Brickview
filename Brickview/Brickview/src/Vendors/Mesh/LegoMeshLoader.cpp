#include "Pch.h"
#include "LegoMeshLoader.h"

#include "Utils/StringUtils.h"
#include "LDrawReader.h"

#include <glm/glm.hpp>

namespace Brickview
{

	struct LegoMeshLoaderData
	{
		std::filesystem::path BaseDirectory;
		std::filesystem::path PartsDirectory;
		std::filesystem::path SubPartsDirectory;
		std::filesystem::path PrimitivesDirectory;
	};

	static LegoMeshLoaderData* s_legoMeshLoaderData = nullptr;

	LegoMeshFileData::LegoMeshFileData(const std::filesystem::path& filePath, LegoMeshFileType type, const glm::mat4& transform)
		: FilePath(filePath)
		, Type(type)
		, Transform(transform)
	{}

	namespace Utils
	{

		static std::string fileTypeToString(LegoMeshFileType type)
		{
			switch (type)
			{
				case LegoMeshFileType::None:      return "None";
				case LegoMeshFileType::Part:      return "Parts";
				case LegoMeshFileType::SubPart:   return "Sub-parts";
				case LegoMeshFileType::Primitive: return "Primitive";
			}

			return "Unknown";
		}

	}

// Old implementation
#if 0
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
#endif

	void LegoMeshLoader::init()
	{
		BV_ASSERT(!s_legoMeshLoaderData, "LDrawReader engine already initialized!");

		s_legoMeshLoaderData                      = new LegoMeshLoaderData();
		s_legoMeshLoaderData->BaseDirectory       = "./data/LDraw/";
		s_legoMeshLoaderData->PartsDirectory      = "parts";
		s_legoMeshLoaderData->SubPartsDirectory   = "s";
		s_legoMeshLoaderData->PrimitivesDirectory = "p";
	}

	void LegoMeshLoader::shutdown()
	{
		delete s_legoMeshLoaderData;
		s_legoMeshLoaderData = nullptr;
	}

	bool LegoMeshLoader::load(const std::filesystem::path& filePath, Ref<Mesh> mesh)
	{
		if (!std::filesystem::exists(filePath))
		{
			BV_LOG_ERROR("Couldn't load: {}", filePath.generic_string());
			return false;
		}

		std::queue<LegoMeshFileData> loadingQueue;
		LegoMeshFileData initialFile(filePath, LegoMeshFileType::Part, glm::mat4(1.0f));
		loadingQueue.push(initialFile);

		while (!loadingQueue.empty())
		{
			const LegoMeshFileData& file = loadingQueue.front();
			readFile(file, mesh, loadingQueue);
			loadingQueue.pop();
		}

		convertToDm(mesh);

		return true;
	}

	bool LegoMeshLoader::readFile(const LegoMeshFileData& file, Ref<Mesh> mesh, std::queue<LegoMeshFileData>& loadingQueue)
	{
		std::filesystem::path currentFilePath = file.FilePath;
		LegoMeshFileType currentFileType      = file.Type;
		glm::mat4 currentTransform            = file.Transform;

		LDrawReader reader(currentFilePath);

		if (!reader.isValid())
		{
			BV_LOG_ERROR("Couldn't read file of type {} in {}", Utils::fileTypeToString(currentFileType), currentFilePath.generic_string());
			return false;
		}

		while (reader.readLine())
		{
			LineType lineType = reader.getLineType();

			switch (lineType)
			{
				case LineType::Triangle:
				{
					LDrawTriangleData t = reader.getLineData<LDrawTriangleData>();
					mesh->addTriangle(t.p0, t.p1, t.p2, currentTransform);
					break;
				}
				case LineType::Quadrilateral:
				{
					LDrawQuadData q = reader.getLineData<LDrawQuadData>();
					mesh->addQuad(q.p0, q.p1, q.p2, q.p3, currentTransform);
					break;
				}
				case LineType::SubFileRef:
				{
					LDrawSubFileRefData sf = reader.getLineData<LDrawSubFileRefData>();
					LegoMeshFileData newFile;
					newFile.Type      = getChildFileType(currentFileType);
					newFile.FilePath  = getChildFilePath(newFile.Type, sf.FilePath);
					newFile.Transform = currentTransform * sf.Transform;
					loadingQueue.push(newFile);
					break;
				}

				case LineType::Comment:
					BV_LOG_INFO("LDraw file comment: {}", reader.getComment());
					break;
				default:
					BV_LOG_INFO("Ignoring line with line type {}", LDrawReader::lineTypeToString(lineType));
					break;
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

	void LegoMeshLoader::convertToDm(Ref<Mesh> mesh)
	{
		// Convert LDU to mm
		// 1 LDU = 0.4 mm
		mesh->scale(0.4);
		// Convert mm to dm
		// 1 mm = 0.01 dm;
		mesh->scale(0.01);
	}

	std::filesystem::path LegoMeshLoader::getFullPartsDirectory()
	{
		return s_legoMeshLoaderData->BaseDirectory / s_legoMeshLoaderData->PartsDirectory;
	}

	std::filesystem::path LegoMeshLoader::getFullSubPartsDirectory()
	{
		return getFullPartsDirectory() / s_legoMeshLoaderData->SubPartsDirectory;
	}

	std::filesystem::path LegoMeshLoader::getFullPrimitivesDirectory()
	{
		return s_legoMeshLoaderData->BaseDirectory / s_legoMeshLoaderData->PrimitivesDirectory;
	}

	LegoMeshFileType LegoMeshLoader::getChildFileType(LegoMeshFileType parentType)
	{
		if (parentType == LegoMeshFileType::Part)
			return LegoMeshFileType::SubPart;

		if (parentType == LegoMeshFileType::SubPart || parentType == LegoMeshFileType::Primitive)
			return LegoMeshFileType::Primitive;

		return LegoMeshFileType::None;
	}

	std::filesystem::path LegoMeshLoader::getChildFilePath(LegoMeshFileType parentType, std::filesystem::path& fileName)
	{
		switch (parentType)
		{
			case LegoMeshFileType::Part:      return getFullPartsDirectory() / fileName;
			case LegoMeshFileType::SubPart:   return getFullPartsDirectory() / fileName;
			case LegoMeshFileType::Primitive: return getFullPrimitivesDirectory() / fileName;
		}

		BV_ASSERT(false, "Unknown file type!");
		return "";
	}

}