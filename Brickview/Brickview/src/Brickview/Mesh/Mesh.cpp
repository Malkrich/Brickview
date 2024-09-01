#include "Pch.h"
#include "Mesh.h"

#include "Mesh/LegoMeshLoader.h"
#include "Mesh/ObjLoader.h"

namespace Brickview
{
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
				loadSuccess = ObjLoader::load(filePath, vertices, indices);
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

	Ref<Mesh> Mesh::loadLDrawExample()
	{
		Ref<Mesh> ldrawExampleMesh = Mesh::load("data/Models/LDrawExample/parts/1.dat");
		return ldrawExampleMesh;
	}

	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<TriangleFace>& indices)
		: m_vertices(vertices)
		, m_indices(indices)
	{}

}