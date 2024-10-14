#include "Pch.h"
#include "ObjLoader.h"

namespace Brickview
{

	bool operator==(const ObjFace::ObjVertexIndex& o1, const ObjFace::ObjVertexIndex& o2)
	{
		return o1.PositionIndex == o2.PositionIndex && o1.NormalIndex == o2.NormalIndex;
	}

	bool ObjLoader::load(const std::filesystem::path& filePath, Ref<Mesh> mesh)
	{
		if (!std::filesystem::exists(filePath))
			return false;

		std::ifstream file(filePath);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<ObjFace> faces;

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
			if (prefix == "vn")
			{
				glm::vec3 vertexNormal;
				ss >> vertexNormal.x >> vertexNormal.y >> vertexNormal.z;
				normals.push_back(vertexNormal);
			}
			// face
			if (prefix == "f")
			{

				uint32_t index = 0;
				uint32_t faceIndex = 0;
				uint32_t counter = 0;

				ObjFace face = {};
				while (ss >> index)
				{
					if (counter == ObjComponents::Position)
						face.Indices[faceIndex].PositionIndex = index - 1;
					if (counter == ObjComponents::Normal)
						face.Indices[faceIndex].NormalIndex = index - 1;

					if (ss.peek() == '/')
					{
						counter++;
						ss.ignore(1, '/');
					}
					else if (ss.peek() == ' ')
					{
						counter = 0;
						faceIndex++;
						ss.ignore(1, ' ');
					}
				}
				faces.push_back(face);
			}
		}

		std::unordered_map<ObjFace::ObjVertexIndex, uint32_t> indexOfVertex;
		uint32_t currentIndex = 0;

		std::vector<Vertex> vertices;
		std::vector<TriangleFace> indices;
		for (const auto& face : faces)
		{
			TriangleFace f;

			for (uint32_t i = 0; i < 3; i++)
			{
				Vertex v = {
					positions[face.Indices[i].PositionIndex],
					normals[face.Indices[i].NormalIndex]
				};

				if (indexOfVertex.find(face.Indices[i]) == indexOfVertex.end())
				{
					indexOfVertex[face.Indices[i]] = currentIndex;
					f[i] = currentIndex;
					vertices.push_back(v);

					currentIndex++;
				}
				else
					f[i] = indexOfVertex.at(face.Indices[i]);
			}
			indices.push_back(f);
		}

		mesh->setData(vertices, indices);
		return true;
	}

}