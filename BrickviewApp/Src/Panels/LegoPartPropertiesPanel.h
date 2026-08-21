#pragma once

#include "Scene/BrickviewScene.h"

#include "Scene/Entity.h"

namespace Brickview
{

  class LegoPartPropertiesPanel
  {
  public:
    LegoPartPropertiesPanel(Soup::Ref<BrickviewScene> scene);

    Soup::Entity GetEntityContext() const { return m_EntityContext; }
    bool HasEntityContext() const { return (bool)m_EntityContext; }
    void SetEntityContext(Soup::Entity entity) { m_EntityContext = entity; }

    void OnGuiRender();

    void DrawEntityComponents(Soup::Entity entity);

  private:
    Soup::Entity m_EntityContext;
    Soup::Ref<BrickviewScene> m_Scene;
  };

}
