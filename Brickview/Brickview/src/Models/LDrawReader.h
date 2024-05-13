#pragma once

#include "Mesh.h"

#include <filesystem>

namespace Brickview
{

	class LDrawReader
	{
	public:
		static bool load(const std::filesystem::path& filePath, std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices);
	};

}