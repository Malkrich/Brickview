#include <Core/EntryPoint.h>

#include "ApplicationLayer.h"

namespace Brickview
{

  class BrickviewApp : public Soup::Application
  {
  public:
    BrickviewApp(const Soup::ApplicationSpecs& specs)
      : Soup::Application(specs)
    {
      PushLayer<ApplicationLayer>();
    }
  };

}

Soup::Application* Soup::CreateApplication(int argc, char* argv[])
{
  Soup::ApplicationSpecs specs;
  specs.Name = "Brickview";
  return new Brickview::BrickviewApp(specs);
}
