#pragma once

#include "Brickview/Mesh/Mesh.h"
#include "LDrawFileManager.h"

#include <tuple>
#include <filesystem>

namespace Brickview
{

	struct LoadingSettings
	{
		bool Inverted = false;
	};

	struct LoadingQueueFileData
	{
		LoadingQueueFileData() = default;
		LoadingQueueFileData(const std::filesystem::path& filePath, LDrawFileType type, const glm::mat4& transform);

		std::filesystem::path FilePath = "";
		glm::mat4 Transform = glm::mat4(1.0f);
	};

	class LegoMeshLoader
	{
	public:
		static bool load(const std::filesystem::path& filePath, Ref<Mesh> mesh);

	private:
		// Loading
		static bool readFile(const LoadingQueueFileData& file, Ref<Mesh> mesh,
			LoadingSettings& settings, std::queue<LoadingQueueFileData>& loadingQueue);
		static void convertToDm(Ref<Mesh> mesh);
	};

}