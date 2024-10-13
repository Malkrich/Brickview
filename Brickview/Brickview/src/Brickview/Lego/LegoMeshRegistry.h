#pragma once

#include "Renderer/Mesh.h"
#include "Renderer/GpuMesh.h"
#include "LegoPart.h"

namespace Brickview
{

	struct LegoPartMeshData
	{
		Ref<GpuMesh> Mesh = nullptr;

		LegoPartMeshData() = default;
		LegoPartMeshData(Ref<GpuMesh> mesh)
			: Mesh(mesh)
		{}
	};

	class LegoMeshRegistry
	{
	public:
		void addPart(LegoPartID id, Ref<Mesh> mesh);
		const LegoPartMeshData& getPart(LegoPartID id) const;

		std::unordered_map<LegoPartID, LegoPartMeshData>::const_iterator begin() const { return m_registry.begin(); }
		std::unordered_map<LegoPartID, LegoPartMeshData>::const_iterator end() const { return m_registry.end(); }

	private:
		std::unordered_map<LegoPartID, LegoPartMeshData> m_registry;
	};

}