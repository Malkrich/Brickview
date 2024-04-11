#pragma once

#include "Core/Core.h"

namespace Brickview
{

	struct Vertex
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Normal = { 0.0f, 0.0f, 0.0f };
	};

	struct TriangleFace
	{
		unsigned int getIndex1() const { return indices[0]; }
		unsigned int getIndex2() const { return indices[1]; }
		unsigned int getIndex3() const { return indices[2]; }

		unsigned int& operator[](unsigned int i)
		{ 
			BV_ASSERT(i < 3, "Index of a triangle has to be less than 3.");
			return indices[i]; 
		}
		unsigned int operator[](unsigned int i) const
		{
			BV_ASSERT(i < 3, "Index of a triangle has to be less than 3.");
			return indices[i];
		}

	private:
		unsigned int indices[3];
	};

	class Mesh
	{
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<TriangleFace>& indices);

		static std::shared_ptr<Mesh> load(const std::filesystem::path& filePath);

		unsigned int getVertexBufferElementCount() const { return m_vertices.size() * 6; }
		unsigned int getIndexBufferElementCount() const { return m_indices.size() * 3; }

		// TEMP
		void* getVertexBufferData() const { return (void*)m_vertices.data(); }
		void* getIndexBufferData() const { return (void*)m_indices.data(); }

	private:
		std::vector<Vertex> m_vertices;
		std::vector<TriangleFace> m_indices;
	};

}