#include "Pch.h"
#include "LegoMeshRegistry.h"

namespace Brickview
{

	void LegoMeshRegistry::addPart(LegoPartID id, Ref<Mesh> mesh)
	{
		// Early return if already contained
		if (m_registry.contains(id))
			return;

		// Upload to GPU
		LegoPartMeshData meshData = createRef<GpuMesh>(mesh);
		m_registry[id] = meshData;
	}

	const LegoPartMeshData& LegoMeshRegistry::getPart(LegoPartID id) const
	{
		BV_ASSERT(m_registry.contains(id), "Lego Part ID does not exist!");
		return m_registry.at(id);
	}

}