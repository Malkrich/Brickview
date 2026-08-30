#pragma once

#include <Core/Time.h>
#include <GUI/Panel.h>
#include <Renderer/SceneRenderer.h>

namespace Brickview
{

  class RendererSettingsPanel : public Soup::Panel
  {
  public:
    RendererSettingsPanel(Soup::Ref<Soup::SceneRenderer> renderer);

    void SetFrameDelta(Soup::DeltaTime delta) { m_DeltaTime = delta; }

  protected:
    void RenderPanelContent() override;

  private:
    void ImportHdriFile(const std::filesystem::path& filePath);

    Soup::Ref<Soup::SceneRenderer> m_Renderer = nullptr;
    Soup::DeltaTime m_DeltaTime;

    std::filesystem::path m_HdriFilePath;
  };

}
