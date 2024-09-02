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
		static Ref<Mesh> loadLDrawExample();

		Mesh() = default;

		bool isEmpty() const { return m_vertices.empty(); }

		const std::vector<Vertex>& getVertices() const { return m_vertices; }
		const std::vector<TriangleFace>& getIndices() const { return m_indices; }

		void setData(const std::vector<Vertex>& vertices, const std::vector<TriangleFace> indices);
		void addTriangle(
			const glm::vec3& p0, const glm::vec3& n0,
			const glm::vec3& p1, const glm::vec3& n1,
			const glm::vec3& p2, const glm::vec3& n2);
		void addTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);
		void addQuad(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

	private:
		std::vector<Vertex> m_vertices;
		std::vector<TriangleFace> m_indices;
		uint32_t m_lastIndex = 0;
	};

}