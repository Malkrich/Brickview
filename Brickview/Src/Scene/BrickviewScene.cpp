#include "BrickviewScene.h"

#include "BrickviewComponents.h"

#include "Scene/Entity.h"

namespace Brickview
{

  Soup::Entity BrickviewScene::CreateLegoPartEntity(LegoPartID partID, Soup::AssetHandle assetHandle)
  {
    Soup::Entity entity = CreateMeshEntity(assetHandle);

    m_LegoPartMeshRegistry.AddPart(partID, assetHandle);
    entity.AddComponent<LegoPartComponent>(partID);

    return entity;
  }

}
