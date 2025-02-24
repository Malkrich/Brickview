#include "Pch.h"
#include "GpuMesh.h"

namespace Brickview
{

	GpuMesh::GpuMesh(Ref<Mesh> mesh)
	{
		const std::vector<Vertex>& vertices = mesh->getVertices();
		const std::vector<TriangleFace>& indices = mesh->getConnectivities();

		Layout vertexBufferLayout = {
			{ 0, "a_position", BufferElementType::Float3 },
			{ 1, "a_normal", BufferElementType::Float3 }
		};

		createBuffers(vertices.size() * sizeof(Vertex), vertices.data(), vertexBufferLayout, 
			indices.size() * sizeof(TriangleFace), indices.data());
	}

	GpuMesh::GpuMesh(uint32_t vertexBufferSize, const void* vertexBufferData, const Layout& vertexBufferLayout, uint32_t indexBufferSize, const void* indexBufferData)
	{
		createBuffers(vertexBufferSize, vertexBufferData, vertexBufferLayout, indexBufferSize, indexBufferData);
	}

	void GpuMesh::createBuffers(uint32_t vertexBufferSize, const void* vertexBufferData, const Layout& vertexBufferLayout, uint32_t indexBufferSize, const void* indexBufferData)
	{
		m_geometryVbo = VertexBuffer::create(vertexBufferSize, vertexBufferData);
		m_geometryVbo->setBufferLayout(vertexBufferLayout);
		m_geometryEbo = IndexBuffer::create(indexBufferSize, indexBufferData);
	}

}