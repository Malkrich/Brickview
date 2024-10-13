#pragma once

#include "Mesh.h"
#include "Buffer/Buffer.h"

namespace Brickview
{

	class GpuMesh
	{
	public:
		GpuMesh(const Ref<Mesh>& mesh);

		const Ref<VertexBuffer>& getGeometryVertexBuffer() const { return m_geometryVbo; }
		const Ref<IndexBuffer>& getGeometryIndexBuffer() const { return m_geometryEbo; }

	private:
		Ref<VertexBuffer> m_geometryVbo;
		Ref<IndexBuffer> m_geometryEbo;
	};

}