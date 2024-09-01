#pragma once

#include "Brickview/Mesh/Mesh.h"

#include <filesystem>
#include <queue>

namespace Brickview
{

	struct SubMeshData;

	class LegoMeshLoader
	{
	public:
		static void init();
		static void shutdown();

		static bool load(const std::filesystem::path& filePath, Ref<Mesh> mesh);

	private:
		static bool readFile(const SubMeshData& meshData,
			std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices,
			std::queue<SubMeshData>& subMeshes);

		static bool isInPartsDirectory(const std::filesystem::path& filePath);
		static bool isInSubPartsDirectory(const std::filesystem::path& filePath);

		static bool isPrefixACommand(const std::string& prefix);
	};

}