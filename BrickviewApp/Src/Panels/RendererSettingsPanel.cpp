#include "RendererSettingsPanel.h"

#include <IO/TextureReader.h>
#include <Project/Project.h>

namespace Brickview
{

  RendererSettingsPanel::RendererSettingsPanel(Soup::Ref<Soup::SceneRenderer> renderer)
    : m_Renderer(renderer)
  {
    m_HdriFilePath = "./data/HDRI/bambanani_sunset_2k.hdr";
    ImportHdriFile(m_HdriFilePath);
  }

  void RendererSettingsPanel::RenderPanelContent()
  {
    ImGui::SeparatorText("Render statistics:");
    ImGui::Text("ts: %.3f ms", m_DeltaTime.GetMilliseconds());
    ImGui::Text("Fps: %.3f", m_DeltaTime.GetSeconds() == 0.0f ? 0.0f : 1.0f / m_DeltaTime.GetSeconds());

    ImGui::SeparatorText("Renderer Settings:");

    ImGui::Columns(2);
    {
      Soup::SceneRenderer::Settings& rendererSettings = m_Renderer->GetRendererSettings();

      // Render mode
      ImGui::Text("Renderer Type");
      ImGui::NextColumn();
      Soup::RendererPreset rendererType = rendererSettings.RenderingPreset;
      std::vector<std::string> rendererTypeStrings = { "Solid", "Lighted" };
      std::string& selectedRendererTypeString = rendererTypeStrings[(int32_t)rendererType];
      if (ImGui::BeginCombo("##rendererType", selectedRendererTypeString.c_str()))
      {
        for (size_t i = 0; i < rendererTypeStrings.size(); i++)
        {
          bool isSelected = selectedRendererTypeString == rendererTypeStrings[i];
          if (ImGui::Selectable(rendererTypeStrings[i].c_str(), isSelected))
          {
            rendererSettings.RenderingPreset = (Soup::RendererPreset)i;
          }

          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
      }
      ImGui::NextColumn();

      // Selection outline width
      ImGui::Columns(1);
      ImGui::SeparatorText("Outline");
      ImGui::Columns(2);
      ImGui::Text("Line width");
      ImGui::NextColumn();
      ImGui::SliderFloat("##outlineWidthSlider", &rendererSettings.OutlineWidth, 0.01f, 5.0f);
      ImGui::NextColumn();
      ImGui::Text("Outline color");
      ImGui::NextColumn();
      ImGui::ColorEdit3("##outlineColorPicker", glm::value_ptr(rendererSettings.OutlineColor));
      ImGui::NextColumn();

      // Skybox type
      ImGui::Columns(1);
      ImGui::SeparatorText("Environment");
      ImGui::Columns(2);
      ImGui::Text("HDR image: ");
      ImGui::NextColumn();
      std::string strHdriFilePath = m_HdriFilePath.string();
      if (ImGui::InputText("##hdrImagePath", (char*)strHdriFilePath.c_str(), strHdriFilePath.capacity() + 1))
        m_HdriFilePath = strHdriFilePath;
      ImGui::NextColumn();
      ImGui::Columns(1);
      if (ImGui::Button("Import texture"))
      {
        ImportHdriFile(m_HdriFilePath);
      }
      ImGui::Columns(2);

      ImGui::Text("Skybox Type");
      ImGui::NextColumn();
      Soup::RendererBackground background = rendererSettings.Background;
      std::vector<std::string> rendererBackgroundStr = { "Clear color", "Environment Map", "Irradiance Map",
        "PreFiltered Map" };
      const std::string& currentRendererBackground = rendererBackgroundStr[(int32_t)background];
      if (ImGui::BeginCombo("##skyboxType", currentRendererBackground.c_str()))
      {
        for (size_t i = 0; i < rendererBackgroundStr.size(); i++)
        {
          bool isSelected = currentRendererBackground == rendererBackgroundStr[i];
          if (ImGui::Selectable(rendererBackgroundStr[i].c_str(), isSelected))
          {
            rendererSettings.Background = (Soup::RendererBackground)i;
          }

          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
      }
      ImGui::NextColumn();

      // Specular skybox mipmap level
      bool levelOptionEnabled = rendererSettings.Background == Soup::RendererBackground::PreFilteredMap ||
        rendererSettings.Background == Soup::RendererBackground::EnvironmentMap;
      if (!levelOptionEnabled)
        rendererSettings.SkyboxMipFactor = 0.0f;

      ImGui::BeginDisabled(!levelOptionEnabled);
      ImGui::Text("Skybox Mip Factor:");
      ImGui::NextColumn();
      ImGui::SliderFloat("##SkyboxMipFactor", &rendererSettings.SkyboxMipFactor, 0.0f, 1.0f);
      ImGui::NextColumn();
      ImGui::EndDisabled();
    }
    ImGui::Columns(1);
  }

  void RendererSettingsPanel::ImportHdriFile(const std::filesystem::path& filePath)
  {
    // TODO: should be moved to proper application directory. Project should be the opened Brickview project.
    std::filesystem::path fullPath = Soup::Project::GetActiveWorkingDirectory() / filePath;
    Soup::Ref<Soup::Texture2D> hdriTexture =
      std::dynamic_pointer_cast<Soup::Texture2D>(Soup::TextureReader::ImportHDRITexture(fullPath));

    Soup::EnvironmentCreateInfo environmentCreateInfo;
    m_Renderer->CreateEnvironment(hdriTexture, environmentCreateInfo);
  }

}
