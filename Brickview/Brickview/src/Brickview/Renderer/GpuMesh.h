#pragma once

#include "Geometry/Mesh.h"
#include "GpuBuffer.h"

namespace Brickview
{

	class GpuMesh
	{
	public:
		GpuMesh(Ref<Mesh> mesh);
		GpuMesh(uint32_t vertexBufferSize, const void* vertexBufferData, const Layout& vertexBufferLayout, uint32_t indexBufferSize, const void* indexBufferData);

		const Ref<VertexBuffer>& getGeometryVertexBuffer() const { return m_geometryVbo; }
		const Ref<IndexBuffer>& getGeometryIndexBuffer() const { return m_geometryEbo; }

	private:
		void createBuffers(uint32_t vertexBufferSize, const void* vertexBufferData, const Layout& vertexBufferLayout, uint32_t indexBufferSize, const void* indexBufferData);

	private:
		Ref<VertexBuffer> m_geometryVbo;
		Ref<IndexBuffer> m_geometryEbo;
	};

}