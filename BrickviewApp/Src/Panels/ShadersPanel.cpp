#include "ShadersPanel.h"

#include <Renderer/Renderer.h>

namespace Brickview
{

  void ShadersPanel::RenderPanelContent()
  {
    const Soup::Ref<Soup::ShaderLibrary>& shaderLibrary = Soup::Renderer::GetShaderLibrary();
    for (const auto& [name, shaderData] : *shaderLibrary)
    {
      ImGui::Text("%s", name.c_str());
      ImGui::SameLine();
      ImGui::PushID(name.c_str());
      if (ImGui::Button("Reload"))
      {
        std::filesystem::path shaderFilePath = shaderData.FilePath;
        shaderData.ShaderProgram->Reload(shaderFilePath);
      }
      ImGui::PopID();
    }
  }

}
