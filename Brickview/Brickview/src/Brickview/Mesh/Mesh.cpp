#include "Pch.h"
#include "Mesh.h"

#include "Vendors/Mesh/LegoMeshLoader.h"
#include "Vendors/Mesh/ObjLoader.h"

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
		Ref<Mesh> mesh = createRef<Mesh>();

		auto extension = filePath.extension();
		auto modelFileFormat = modelFileFormatFromExtension(extension.string());

		bool loadSuccess = false;
		switch (modelFileFormat)
		{
			case ModelFileFormat::Obj:
				loadSuccess = ObjLoader::load(filePath, mesh);
				break;
			case ModelFileFormat::LDrawDat:
				loadSuccess = LegoMeshLoader::load(filePath, mesh);
				break;
		}
		if (!loadSuccess)
		{
			BV_LOG_ERROR("Coudln't load {} !", filePath.generic_string());
			return nullptr;
		}

		return mesh;
	}

	Ref<Mesh> Mesh::loadLDrawExample()
	{
		Ref<Mesh> ldrawExampleMesh = Mesh::load("data/Models/LDrawExample/parts/1.dat");
		return ldrawExampleMesh;
	}

	void Mesh::setData(const std::vector<Vertex>& vertices, const std::vector<TriangleFace> indices)
	{
		m_vertices = vertices;
		m_indices = indices;
		m_lastIndex = vertices.size();
	}

	void Mesh::addTriangle(
		const glm::vec3& p0, const glm::vec3& n0,
		const glm::vec3& p1, const glm::vec3& n1,
		const glm::vec3& p2, const glm::vec3& n2)
	{
		// Geometry
		m_vertices.push_back({ p0, n0 });
		m_vertices.push_back({ p1, n1 });
		m_vertices.push_back({ p2, n2 });

		// Connectivity
		TriangleFace f = { 0, 1, 2 };
		f.addOffset(m_lastIndex);
		m_indices.push_back(f);
		m_lastIndex += 3;
	}

	void Mesh::addTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
	{
		glm::vec3 n = glm::normalize(glm::cross(p2 - p0, p1 - p0));
		addTriangle(
			p0, n,
			p1, n,
			p2, n);
	}

	void Mesh::addQuad(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
	{
		addTriangle(p0, p1, p2);
		addTriangle(p0, p2, p3);
	}

}