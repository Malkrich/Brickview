#pragma once

#include "Scene/BrickviewScene.h"

#include "GUI/Panel.h"

namespace Brickview
{

  class ScenePartsCollectionPanel : public Soup::Panel
  {
  public:
    ScenePartsCollectionPanel(Soup::Ref<BrickviewScene> context);

    const Soup::Ref<BrickviewScene>& GetContext() const { return m_SceneContext; }

  protected:
    void RenderPanelContent() override;

  private:
    Soup::Ref<BrickviewScene> m_SceneContext;
  };

}
