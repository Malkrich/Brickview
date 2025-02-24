#include "Pch.h"
#include "ObjLoader.h"

// From Vendors/OBJ-Loader/Source/
#include "OBJ_Loader.h"

namespace Brickview
{

	bool ObjLoader::load(const std::filesystem::path& filePath, Ref<Mesh> mesh)
	{
		objl::Loader objLoader;
		bool loadResult = objLoader.LoadFile(filePath.generic_string());

		if (loadResult)
		{
			// Vertices
			std::vector<Vertex> vertices;
			const std::vector<objl::Vertex>& nativeVertices = objLoader.LoadedVertices;
			vertices.reserve(nativeVertices.size());
			for (const objl::Vertex& nativeVert : nativeVertices)
			{
				Vertex vert;
				vert.Position = glm::vec3(nativeVert.Position.X, nativeVert.Position.Y, nativeVert.Position.Z);
				vert.Normal = glm::vec3(nativeVert.Normal.X, nativeVert.Normal.Y, nativeVert.Normal.Z);
				vertices.push_back(vert);
			}

			// Indices
			const std::vector<unsigned int>& nativeIndices = objLoader.LoadedIndices;
			std::vector<TriangleFace> indices;
			indices.resize(nativeIndices.size() / 3);
			// The layout between the native indices buffer and the Brickview::Mesh indices buffer are similar
			// so we can just copy it
			memcpy(indices.data(), nativeIndices.data(), nativeIndices.size() * sizeof(unsigned int));

			mesh->setData(vertices, indices);
		}

		return loadResult;
	}

}