#include "Pch.h"
#include "Mesh.h"

#include "Vendors/Mesh/LegoMeshLoader.h"
#include "Vendors/Mesh/ObjLoader.h"
#include "Utils/MathUtils.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Brickview
{
	namespace Utils
	{
		static glm::vec3 computeNormal(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::mat4& geometryTransform)
		{
			glm::vec3 normal = glm::cross(p2 - p0, p1 - p0);

			glm::mat3 normalTransform = MathUtils::computeNormalTransform(geometryTransform);

			return glm::normalize(normalTransform * normal);
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

	void Mesh::setData(const std::vector<Vertex>& vertices, const std::vector<TriangleFace> connectivities)
	{
		m_vertices = vertices;
		m_connectivities = connectivities;
		m_lastIndex = vertices.size();
	}

	void Mesh::addTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
	{
		addGeometry<3, 1>({ v0, v1, v2 });
	}

	void Mesh::addTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::mat4& transform)
	{
		glm::vec3 tp0 = transform * glm::vec4(p0, 1.0f);
		glm::vec3 tp1 = transform * glm::vec4(p1, 1.0f);
		glm::vec3 tp2 = transform * glm::vec4(p2, 1.0f);

		// Normal calculation
		glm::vec3 normal = Utils::computeNormal(p0, p1, p2, transform);

		Vertex v0 = { tp0, normal };
		Vertex v1 = { tp1, normal };
		Vertex v2 = { tp2, normal };

		addTriangle(v0, v1, v2);
	}

	void Mesh::addTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
	{
		addTriangle(p0, p1, p2, glm::mat4(1.0f));
	}

	void Mesh::addQuad(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3)
	{
		addGeometry<4, 2>({ v0, v1, v2, v3 });
	}

	void Mesh::addQuad(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::mat4& transform)
	{
		glm::vec3 tp0 = transform * glm::vec4(p0, 1.0f);
		glm::vec3 tp1 = transform * glm::vec4(p1, 1.0f);
		glm::vec3 tp2 = transform * glm::vec4(p2, 1.0f);
		glm::vec3 tp3 = transform * glm::vec4(p3, 1.0f);

		// normal calculation
		glm::vec3 normal = Utils::computeNormal(p0, p1, p2, transform);

		Vertex v0 = { tp0, normal };
		Vertex v1 = { tp1, normal };
		Vertex v2 = { tp2, normal };
		Vertex v3 = { tp3, normal };

		addQuad(v0, v1, v2, v3);
	}

	void Mesh::addQuad(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
	{
		addQuad(p0, p1, p2, p3, glm::mat4(1.0f));
	}

	void Mesh::transform(const glm::mat4& transform)
	{
		glm::mat3 normalTransform = MathUtils::computeNormalTransform(transform);

		for (Vertex& v : m_vertices)
		{
			v.Position = transform * glm::vec4(v.Position, 1.0f);
			v.Normal = glm::normalize(normalTransform * v.Normal);
		}
	}

	void Mesh::scale(float scaleFactor)
	{
		scale(glm::vec3(scaleFactor));
	}

	void Mesh::scale(const glm::vec3& scaleVector)
	{
		glm::mat4 scaleTransform = glm::scale(glm::mat4(1.0), scaleVector);
		transform(scaleTransform);
	}

}