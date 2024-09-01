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
		TriangleFace(uint32_t i1, uint32_t i2, uint32_t i3)
			: m_indices{ i1, i2, i3 }
		{}
		TriangleFace()
			: m_indices{ 0, 0, 0 }
		{}

		uint32_t getIndex1() const { return m_indices[0]; }
		uint32_t getIndex2() const { return m_indices[1]; }
		uint32_t getIndex3() const { return m_indices[2]; }

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

		void setData(const std::vector<Vertex>& vertices, const std::vector<TriangleFace> indices)
		{
			m_vertices = vertices;
			m_indices = indices;
		}

	private:
		std::vector<Vertex> m_vertices;
		std::vector<TriangleFace> m_indices;
	};

}