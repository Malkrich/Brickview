

#include "LegoPartMeshRegistry.h"

namespace Brickview
{

  void LegoPartMeshRegistry::AddPart(LegoPartID id, Soup::Ref<Soup::Mesh> mesh)
  {
    if (m_Registry.contains(id))
      return;

    m_Registry[id] = CreateRef<Soup::GpuMesh>(mesh);
  }

  Soup::Ref<Soup::GpuMesh> LegoPartMeshRegistry::GetPart(LegoPartID id) const
  {
    SP_ASSERT(m_Registry.contains(id), "Lego Part ID does not exist!");
    return m_Registry.at(id);
  }

}
