#pragma once

#include "Scene/BrickviewScene.h"

namespace Brickview
{

  class ScenePartsListPanel
  {
  public:
    ScenePartsListPanel(Soup::Ref<BrickviewScene> context);

    const Soup::Ref<BrickviewScene>& GetContext() const { return m_SceneContext; }

    void OnGuiRender();

  private:
    Soup::Ref<BrickviewScene> m_SceneContext;
  };

}
