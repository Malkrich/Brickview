#pragma once

#include "Geometry/Mesh.h"
#include "LegoPart.h"
#include "Renderer/GpuMesh.h"

namespace Brickview
{

  class LegoPartMeshRegistry
  {
  public:
    void AddPart(LegoPartID id, Soup::Ref<Soup::Mesh> mesh);
    Soup::Ref<Soup::GpuMesh> GetPart(LegoPartID id) const;

    std::unordered_map<LegoPartID, Soup::Ref<Soup::GpuMesh>>::const_iterator begin() const
    {
      return m_Registry.begin();
    }
    std::unordered_map<LegoPartID, Soup::Ref<Soup::GpuMesh>>::const_iterator end() const { return m_Registry.end(); }

  private:
    std::unordered_map<LegoPartID, Soup::Ref<Soup::GpuMesh>> m_Registry;
  };

}
