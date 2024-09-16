#pragma once

#include <glm/glm.hpp>

#include <filesystem>

namespace Brickview
{

	enum class ModelFileFormat
	{
		None = 0,
		Obj,
		LDrawDat
	};

	struct Vertex
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Normal = { 0.0f, 0.0f, 0.0f };
	};

	struct TriangleFace
	{
		TriangleFace(uint32_t i0, uint32_t i1, uint32_t i2)
			: m_indices{ i0, i1, i2 }
		{}
		TriangleFace()
			: m_indices{ 0, 0, 0 }
		{}

		void addOffset(uint32_t offset)
		{
			for (uint32_t& i : m_indices)
				i += offset;
		}

		uint32_t& operator[](uint32_t i)
		{ 
			BV_ASSERT(i < m_indices.size(), "Index of a triangle has to be less than 3.");
			return m_indices[i];
		}
		uint32_t operator[](uint32_t i) const
		{
			BV_ASSERT(i < m_indices.size(), "Index of a triangle has to be less than 3.");
			return m_indices[i];
		}

	private:
		std::array<uint32_t, 3> m_indices;
	};

	class Mesh
	{
	public:
		static Ref<Mesh> load(const std::filesystem::path& filePath);

		Mesh() = default;

		bool isEmpty() const { return m_vertices.empty(); }
		const std::vector<Vertex>& getVertices() const { return m_vertices; }
		const std::vector<TriangleFace>& getConnectivities() const { return m_connectivities; }

		// Data modifications
		void setData(const std::vector<Vertex>& vertices, const std::vector<TriangleFace> indices);
		void addTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);
		void addTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::mat4& trasnform);
		void addQuad(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
		void addQuad(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::mat4& trasnform);

		// Geometry modifications
		void scale(float scaleFactor);
		void scale(const glm::vec3& scaleVector);

	private:
		template<uint32_t VertexCount, uint32_t TriangleCount>
		void addGeometry(const std::array<glm::vec3, VertexCount>& positions, const glm::mat4& transform)
		{
			BV_ASSERT(TriangleCount <= 2, "Geometry which is not triangle nor quad is not supported!");
			// Normal calculation
			glm::vec3 normal = glm::normalize(glm::cross(positions[2] - positions[0], positions[1] - positions[0]));

			// Geometry
			m_vertices.reserve(VertexCount);
			for (uint32_t i = 0; i < VertexCount; i++)
			{
				glm::vec3 pos = glm::vec3(transform * glm::vec4(positions[i], 1.0f));
				Vertex v = { pos, normal };
				m_vertices.push_back(v);
			}

			// Connectivity
			m_connectivities.reserve(TriangleCount);
			TriangleFace t1 = { 0, 1, 2 };
			t1.addOffset(m_lastIndex);
			m_connectivities.push_back(t1);
			if constexpr(TriangleCount > 1)
			{
				TriangleFace t2 = { 2, 3, 0 };
				t2.addOffset(m_lastIndex);
				m_connectivities.push_back(t2);
			}

			m_lastIndex += VertexCount;
		}

	private:
		std::vector<Vertex> m_vertices;
		std::vector<TriangleFace> m_connectivities;
		uint32_t m_lastIndex = 0;
	};

}