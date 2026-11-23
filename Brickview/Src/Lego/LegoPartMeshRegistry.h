#pragma once

#include "LegoPart.h"

#include <Asset/Asset.h>

namespace Brickview
{

  class LegoPartMeshRegistry
  {
  public:
    void AddPart(LegoPartID partId, Soup::AssetHandle assetHandle);
    Soup::AssetHandle GetPart(LegoPartID partId) const;

    std::unordered_map<LegoPartID, Soup::AssetHandle>::const_iterator begin() const { return m_Registry.begin(); }
    std::unordered_map<LegoPartID, Soup::AssetHandle>::const_iterator end() const { return m_Registry.end(); }

  private:
    std::unordered_map<LegoPartID, Soup::AssetHandle> m_Registry;
  };

}
