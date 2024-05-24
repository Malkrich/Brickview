#include "Pch.h"
#include "Mesh.h"
#include "Models/LDraw/LegoMeshLoader.h"

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
				uint32_t PositionIndex;
				// TODO : unsigned int TextureIndex;
				uint32_t NormalIndex;
			};

			ObjVertexIndex Indices[3];
		};

		bool operator<(const ObjFace::ObjVertexIndex& a, const ObjFace::ObjVertexIndex& b)
		{
			return std::make_pair(a.PositionIndex, a.NormalIndex) < std::make_pair(b.PositionIndex, b.NormalIndex);
		}

		bool loadObj(const std::filesystem::path& filePath, std::vector<Vertex>& vertices, std::vector<TriangleFace>& indices)
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

			std::map<ObjFace::ObjVertexIndex, uint32_t> indexOfVertex;
			uint32_t currentIndex = 0;

			for (const auto& face : faces)
			{
				TriangleFace f;

				for (uint32_t i = 0; i < 3; i++)
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

	static std::unordered_map<std::string, ModelFileFormat> fileFormatMap = {
		{ ".obj", ModelFileFormat::Obj },
		{ ".dat", ModelFileFormat::LDrawDat }
	};

	static ModelFileFormat modelFileFormatFromExtension(const std::string& extension)
	{
		if (fileFormatMap.find(extension) == fileFormatMap.end())
			return ModelFileFormat::None;

		return fileFormatMap.at(extension);
	}

	Ref<Mesh> Mesh::load(const std::filesystem::path& filePath)
	{
		std::vector<Vertex> vertices;
		std::vector<TriangleFace> indices;

		auto extension = filePath.extension();
		auto modelFileFormat = modelFileFormatFromExtension(extension.string());

		bool loadSuccess = false;
		switch (modelFileFormat)
		{
			case ModelFileFormat::Obj:
				loadSuccess = Utils::loadObj(filePath, vertices, indices);
				break;
			case ModelFileFormat::LDrawDat:
				loadSuccess = LegoMeshLoader::load(filePath, vertices, indices);
				break;
		}
		if (!loadSuccess)
		{
			BV_LOG_ERROR("Coudln't load {} !", filePath.generic_string());
			return nullptr;
		}

		Ref<Mesh> mesh = createRef<Mesh>(vertices, indices);
		return mesh;
	}

	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<TriangleFace>& indices)
		: m_vertices(vertices)
		, m_indices(indices)
	{}

}