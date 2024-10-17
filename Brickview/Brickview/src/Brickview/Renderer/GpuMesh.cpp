#include "Pch.h"
#include "GpuMesh.h"

namespace Brickview
{

	GpuMesh::GpuMesh(const Ref<Mesh>& mesh)
	{
		const std::vector<Vertex>& meshData = mesh->getVertices();
		const std::vector<TriangleFace>& meshConnectivities = mesh->getConnectivities();

		Layout meshGeoLayout = {
			{ 0, "a_position", BufferElementType::Float3 },
			{ 1, "a_normal", BufferElementType::Float3 }
		};

		m_geometryVbo = VertexBuffer::create(
			meshData.size() * sizeof(Vertex),
			(void*)meshData.data());
		m_geometryVbo->setBufferLayout(meshGeoLayout);

		m_geometryEbo = IndexBuffer::create(
			meshConnectivities.size() * sizeof(TriangleFace),
			(void*)meshConnectivities.data());
	}

}