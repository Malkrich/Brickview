#pragma once

#include "GUI/Panel.h"
#include "Renderer/Texture2D.h"

namespace Brickview
{

  class LegoPartsExplorerPanel : public Soup::Panel
  {
  public:
    typedef std::function<void(const std::filesystem::path&)> OnFileLoadCallbackFn;

  public:
    LegoPartsExplorerPanel(const std::filesystem::path& partsDirectory);

    void SetOnLoadCallbackFunction(OnFileLoadCallbackFn fn) { m_OnFileLoadCallback = fn; }

  protected:
    void RenderPanelContent() override;

  private:
    uint32_t m_FileOffset = 2000;
    uint32_t m_MaxDisplayedFile = 10;
    std::filesystem::path m_LegoPartsDirectory;

    OnFileLoadCallbackFn m_OnFileLoadCallback;

    Soup::Ref<Soup::Texture2D> m_DefaultLegoPartIcon;
    uint32_t m_IconWidth = 120;
    uint32_t m_IconPadding = 10;
  };

}
