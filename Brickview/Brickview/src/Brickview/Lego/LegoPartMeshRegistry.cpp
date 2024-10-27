#include "Pch.h"
#include "LegoPartMeshRegistry.h"

namespace Brickview
{

	void LegoPartMeshRegistry::addPart(LegoPartID id, Ref<Mesh> mesh)
	{
		if (m_registry.contains(id))
			return;

		m_registry[id] = createRef<GpuMesh>(mesh);
	}

	Ref<GpuMesh> LegoPartMeshRegistry::getPart(LegoPartID id) const
	{
		BV_ASSERT(m_registry.contains(id), "Lego Part ID does not exist!");
		return m_registry.at(id);
	}

}