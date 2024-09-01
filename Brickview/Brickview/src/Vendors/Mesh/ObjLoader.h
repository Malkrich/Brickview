#pragma once

#include <filesystem>

#include "Mesh/Mesh.h"

namespace Brickview
{

	class ObjLoader
	{
	public:
		static bool load(const std::filesystem::path& filePath, std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices);
	};

}