#pragma once

#include "Brickview/Mesh/Mesh.h"

#include <filesystem>
#include <queue>

namespace Brickview
{

	struct LDrawSubFileRefData;

	class LegoMeshLoader
	{
	public:
		static void init();
		static void shutdown();

		static bool load(const std::filesystem::path& filePath, Ref<Mesh> mesh);

	private:
		static bool readFile(const LDrawSubFileRefData& file, Ref<Mesh> mesh, std::queue<LDrawSubFileRefData>& loadingQueue);
		static void convertToCm(Ref<Mesh> mesh);

		static bool isInPartsDirectory(const std::filesystem::path& filePath);
		static bool isInSubPartsDirectory(const std::filesystem::path& filePath);

		static bool isPrefixACommand(const std::string& prefix);
	};

}