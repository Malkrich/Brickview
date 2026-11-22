#include <Core/EntryPoint.h>

#include "ApplicationLayer.h"
#include "LDraw/LDrawCore.h"

namespace Brickview
{

  class BrickviewApp : public Soup::Application
  {
  public:
    BrickviewApp(const Soup::ApplicationSpecs& specs, const std::filesystem::path& projectPath)
      : Soup::Application(specs)
    {
      // TODO: file path to the lego part data base should be handled by the application, not the active project.
      std::filesystem::path legoPartDataDirectory = projectPath.parent_path() / "data/ldraw";
      LDrawCore::Init(legoPartDataDirectory);
      PushLayer<ApplicationLayer>(projectPath);
    }

    ~BrickviewApp() { LDrawCore::Shutdown(); }
  };

}

Soup::Application* Soup::CreateApplication(int argc, char* argv[])
{
  Soup::ApplicationSpecs specs;
  specs.Name = "Brickview";

  std::filesystem::path projectPath = argv[1];

  return new Brickview::BrickviewApp(specs, projectPath);
}
