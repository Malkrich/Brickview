#pragma once

#include "Models/Mesh.h"

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

		static bool load(const std::filesystem::path& filePath, std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices);

	private:
		static bool readFile(const SubMeshData& meshData,
			std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices,
			std::queue<SubMeshData>& subMeshes);

		static bool isInPartsDirectory(const std::filesystem::path& filePath);
		static bool isInSubPartsDirectory(const std::filesystem::path& filePath);

		static bool isPrefixACommand(const std::string& prefix);
	};

}