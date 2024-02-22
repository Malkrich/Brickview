#pragma once

namespace Brickview
{

	class ObjLoader
	{
	public:
		static bool loadObj(const std::string& filePath,
			std::vector<glm::vec3>& positions,
			std::vector<unsigned int>& indices);
	};

}