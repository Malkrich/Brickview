#include "LegoPartMeshRegistry.h"

namespace Brickview
{

  void LegoPartMeshRegistry::AddPart(LegoPartID partId, Soup::AssetHandle assetHandle)
  {
    m_Registry[partId] = assetHandle;
  }

  Soup::AssetHandle LegoPartMeshRegistry::GetPart(LegoPartID partId) const
  {
    auto it = m_Registry.find(partId);
    return it != m_Registry.end() ? it->second : Soup::AssetHandle(0);
  }

}
