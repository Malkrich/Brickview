#pragma once

#include "Brickview/Mesh/Mesh.h"

#include <filesystem>
#include <queue>

namespace Brickview
{

	enum class LegoMeshFileType
	{
		None = 0,
		Part,
		SubPart,
		Primitive
	};

	struct LegoMeshFileData
	{
		LegoMeshFileData() = default;
		LegoMeshFileData(const std::filesystem::path& filePath, LegoMeshFileType type, const glm::mat4& transform);

		std::filesystem::path FilePath = "";
		LegoMeshFileType Type = LegoMeshFileType::None;
		glm::mat4 Transform = glm::mat4(1.0f);
	};

	class LegoMeshLoader
	{
	public:
		static void init();
		static void shutdown();

		static bool load(const std::filesystem::path& filePath, Ref<Mesh> mesh);

	private:
		static bool readFile(const LegoMeshFileData& file, Ref<Mesh> mesh, std::queue<LegoMeshFileData>& loadingQueue);
		static void convertToDm(Ref<Mesh> mesh);

		static std::filesystem::path getFullPartsDirectory();
		static std::filesystem::path getFullSubPartsDirectory();
		static std::filesystem::path getFullPrimitivesDirectory();
		static LegoMeshFileType getChildFileType(LegoMeshFileType parentType);
		static std::filesystem::path getChildFilePath(LegoMeshFileType parentType, std::filesystem::path& fileName);
	};

}