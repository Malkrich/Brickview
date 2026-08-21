#pragma once

#include "Geometry/Mesh.h"
#include "Lego/LegoPart.h"
#include "Lego/LegoPartMeshRegistry.h"

#include "Scene/Scene.h"

namespace Brickview
{

  class BrickviewScene : public Soup::Scene
  {
  public:
    Soup::Entity CreateLegoPartEntity(LegoPartID partID, Soup::Ref<Soup::Mesh> mesh);

    const LegoPartMeshRegistry& GetLegoPartRegistry() const { return m_LegoPartMeshRegistry; }

  private:
    LegoPartMeshRegistry m_LegoPartMeshRegistry;
  };

}
