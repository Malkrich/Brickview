#pragma once

#include "Mesh.h"

#include <filesystem>
#include <queue>

namespace Brickview
{

	struct SubMeshData;

	class LDrawReader
	{
	public:
		static void init();
		static void shutdown();

		static bool load(const std::filesystem::path& filePath, std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices);

	private:
		static bool readFile(const std::filesystem::path& filePath, const glm::mat4& transform,
			std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices,
			std::queue<SubMeshData>& subMeshes);

	};

}