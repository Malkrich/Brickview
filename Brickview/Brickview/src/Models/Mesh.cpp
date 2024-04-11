#include "Pch.h"
#include "Mesh.h"

#include <sstream>

#define POSITION_IDX 0
#define TEXTURE_IDX  1
#define NORMAL_IDX   2

namespace Brickview
{

	namespace Utils
	{

		struct ObjFace
		{
			struct ObjVertexIndex
			{
				unsigned int PositionIndex;
				// TODO : unsigned int TextureIndex;
				unsigned int NormalIndex;
			};

			ObjVertexIndex Indices[3];
		};

		bool operator<(const ObjFace::ObjVertexIndex& a, const ObjFace::ObjVertexIndex& b)
		{
			return std::make_pair(a.PositionIndex, a.NormalIndex) < std::make_pair(b.PositionIndex, b.NormalIndex);
		}

		bool loadObj(const std::filesystem::path& filePath, std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices)
		{
			std::ifstream file(filePath);

			if (!file.is_open())
			{
				BV_LOG_ERROR("File \"{}\" doesnt exist !", filePath.generic_string());
				return false;
			}

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

					unsigned int index = 0;
					unsigned int faceIndex = 0;
					unsigned int counter = 0;

					ObjFace face = {};
					while (ss >> index)
					{
						if (counter == POSITION_IDX)
							face.Indices[faceIndex].PositionIndex = index - 1;
						if (counter == NORMAL_IDX)
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

			std::map<ObjFace::ObjVertexIndex, unsigned int> indexOfVertex;
			unsigned int currentIndex = 0;

			for (const auto& face : faces)
			{
				TriangleFace f;

				for (unsigned int i = 0; i < 3; i++)
				{
					Vertex v = {
						positions[ face.Indices[i].PositionIndex ], 
						normals[ face.Indices[i].NormalIndex ]
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

			return true;
		}

	}

	std::shared_ptr<Mesh> Mesh::load(const std::filesystem::path& filePath)
	{
		std::vector<Vertex> vertices;
		std::vector<TriangleFace> indices;

		if (!Utils::loadObj(filePath, vertices, indices))
		{
			BV_LOG_ERROR("Coudln't load {} !", filePath.generic_string());
			return nullptr;
		}

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(vertices, indices);
		return mesh;
	}

	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<TriangleFace>& indices)
		: m_vertices(vertices)
		, m_indices(indices)
	{}

}