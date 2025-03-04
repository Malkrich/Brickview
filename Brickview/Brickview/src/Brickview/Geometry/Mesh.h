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

		Vertex() = default;
		Vertex(const Vertex&) = default;
		Vertex(const glm::vec3& position, const glm::vec3& normal)
			: Position(position)
			, Normal(normal) {}
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

		void addTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
		void addTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::mat4& transform);
		void addTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);

		void addQuad(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3);
		void addQuad(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::mat4& transform);
		void addQuad(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

		// Geometry modifications
		void transform(const glm::mat4& transform);
		void scale(float scaleFactor);
		void scale(const glm::vec3& scaleVector);

	private:
		template<uint32_t VertexCount, uint32_t TriangleCount>
		void addGeometry(const std::array<Vertex, VertexCount>& vertices)
		{
			BV_ASSERT(TriangleCount <= 2, "Only triangle and quand are the accepted geometry to be added in Mesh!");

			// Geometry
			m_vertices.reserve(m_vertices.size() + VertexCount);
			for (uint32_t i = 0; i < VertexCount; i++)
				m_vertices.push_back(vertices[i]);

			// Connectivities
			// Triangle 1
			m_connectivities.reserve(m_connectivities.size() + TriangleCount);
			TriangleFace t1 = { 0, 1, 2 };
			t1.addOffset(m_lastIndex);
			m_connectivities.push_back(t1);
			// Triangle 2 -> if adding a quad
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