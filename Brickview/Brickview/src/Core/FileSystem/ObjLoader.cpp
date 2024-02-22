#include "Pch.h"
#include "ObjLoader.h"

#include <sstream>

#include "Core/Log.h"

namespace Brickview
{

	bool ObjLoader::loadObj(const std::string& filePath,
		std::vector<glm::vec3>& positions,
		std::vector<unsigned int>& indices)
	{
		std::ifstream file(filePath.c_str());
		
		if (!file.is_open())
		{
			BV_LOG_ERROR("File \"{}\" doesnt exist !", filePath.c_str());
			return false;
		}

		std::stringstream ss;
		std::string line;
		while (std::getline(file, line))
		{
			ss.clear();
			ss.str(line);
			std::string prefix;
			ss >> prefix;

			// vertex position
			if (prefix == "v")
			{
				glm::vec3 vertexPos;
				ss >> vertexPos.x >> vertexPos.y >> vertexPos.z;
				positions.push_back(vertexPos);
			}
			// face
			if (prefix == "f")
			{
				unsigned int index;
				unsigned int counter = 0;
				while (ss >> index)
				{
					if (counter == 0)
						indices.push_back(index - 1);

					if (ss.peek() == '/')
					{
						counter++;
						ss.ignore(1, '/');
					}
					else if (ss.peek() == ' ')
					{
						counter = 0;
						ss.ignore(1, ' ');
					}
				}
			}
		}

		return true;
	}

}