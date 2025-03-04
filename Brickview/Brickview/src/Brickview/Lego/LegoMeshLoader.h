#pragma once

#include "Brickview/Geometry/Mesh.h"

#include <tuple>
#include <filesystem>

enum class LDrawFileType;

namespace Brickview
{

	struct LoadingQueueFileData
	{
		LoadingQueueFileData() = default;
		LoadingQueueFileData(const std::filesystem::path& filePath, const glm::mat4& transform);

		std::filesystem::path FilePath = "";
		glm::mat4 Transform = glm::mat4(1.0f);
		bool Inverted = false;
	};

	class LegoMeshLoader
	{
	public:
		static void init();
		static void shutdown();

		static bool load(const std::filesystem::path& filePath, Ref<Mesh> mesh);

	private:
		// Loading
		static bool readFile(const LoadingQueueFileData& file, Ref<Mesh> mesh, std::queue<LoadingQueueFileData>& loadingQueue);
	};

}