#pragma once

#include <filesystem>

#include "Mesh/Mesh.h"

namespace Brickview
{

	class ObjLoader
	{
	public:
		static bool load(const std::filesystem::path& filePath, Ref<Mesh> mesh);
	};

}