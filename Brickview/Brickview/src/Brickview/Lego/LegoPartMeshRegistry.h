#pragma once

#include "Geometry/Mesh.h"
#include "Renderer/GpuMesh.h"
#include "LegoPart.h"

namespace Brickview
{

	class LegoPartMeshRegistry
	{
	public:
		void addPart(LegoPartID id, Ref<Mesh> mesh);
		Ref<GpuMesh> getPart(LegoPartID id) const;

		std::unordered_map<LegoPartID, Ref<GpuMesh>>::const_iterator begin() const { return m_registry.begin(); }
		std::unordered_map<LegoPartID, Ref<GpuMesh>>::const_iterator end() const { return m_registry.end(); }

	private:
		std::unordered_map<LegoPartID, Ref<GpuMesh>> m_registry;
	};

}