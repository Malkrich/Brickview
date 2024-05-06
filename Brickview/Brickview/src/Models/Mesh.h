#pragma once

#include <glm/glm.hpp>

#include <filesystem>

namespace Brickview
{

	struct Vertex
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Normal = { 0.0f, 0.0f, 0.0f };
	};

	struct TriangleFace
	{
		uint32_t getIndex1() const { return indices[0]; }
		uint32_t getIndex2() const { return indices[1]; }
		uint32_t getIndex3() const { return indices[2]; }

		uint32_t& operator[](uint32_t i)
		{ 
			BV_ASSERT(i < 3, "Index of a triangle has to be less than 3.");
			return indices[i]; 
		}
		uint32_t operator[](uint32_t i) const
		{
			BV_ASSERT(i < 3, "Index of a triangle has to be less than 3.");
			return indices[i];
		}

	private:
		uint32_t indices[3];
	};

	class Mesh
	{
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<TriangleFace>& indices);

		static Ref<Mesh> load(const std::filesystem::path& filePath);

		const std::vector<Vertex>& getVertices() const { return m_vertices; }
		const std::vector<TriangleFace>& getIndices() const { return m_indices; }

	private:
		std::vector<Vertex> m_vertices;
		std::vector<TriangleFace> m_indices;
	};

}