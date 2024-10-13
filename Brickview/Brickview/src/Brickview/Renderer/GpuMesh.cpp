#include "Pch.h"
#include "GpuMesh.h"

#include "Buffer/Layout.h"

namespace Brickview
{

	GpuMesh::GpuMesh(const Ref<Mesh>& mesh)
	{
		const std::vector<Vertex>& meshData = mesh->getVertices();
		const std::vector<TriangleFace>& meshConnectivities = mesh->getConnectivities();

		Layout meshGeoLayout = {
			{ "a_position", BufferElementType::Float3 },
			{ "a_normal", BufferElementType::Float3 } };

		m_geometryVbo = createRef<VertexBuffer>(
			meshData.size() * sizeof(Vertex),
			(void*)meshData.data());
		m_geometryVbo->setBufferLayout(meshGeoLayout);

		m_geometryEbo = createRef<IndexBuffer>(
			meshConnectivities.size() * sizeof(TriangleFace),
			(void*)meshConnectivities.data());
	}

}