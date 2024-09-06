#include "Pch.h"
#include "Mesh.h"

#include "Vendors/Mesh/LegoMeshLoader.h"
#include "Vendors/Mesh/ObjLoader.h"

#include <glm/gtc/matrix_transform.hpp>

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
		Ref<Mesh> ldrawExampleMesh = Mesh::load("data/LDraw/parts/u9495.dat");
		return ldrawExampleMesh;
	}

	void Mesh::setData(const std::vector<Vertex>& vertices, const std::vector<TriangleFace> connectivities)
	{
		m_vertices = vertices;
		m_connectivities = connectivities;
		m_lastIndex = vertices.size();
	}

	void Mesh::addTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
	{
		addGeometry<3, 1>({ p0, p1, p2 });
	}

	void Mesh::addQuad(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
	{
		addGeometry<4, 2>({ p0, p1, p2, p3 });
	}

	void Mesh::scale(float scaleFactor)
	{
		scale(glm::vec3(scaleFactor));
	}

	void Mesh::scale(const glm::vec3& scaleVector)
	{
		glm::mat4 scaleTransform = glm::scale(glm::mat4(1.0), scaleVector);
		for (Vertex& v : m_vertices)
		{
			glm::vec4 pos = glm::vec4(v.Position, 1.0f);
			pos = scaleTransform * pos;
			v.Position = glm::vec3(pos.x, pos.y, pos.z);
		}
	}

}