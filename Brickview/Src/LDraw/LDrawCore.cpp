

#include "LDrawCore.h"

#include "LDrawCommandManager.h"
#include "LDrawFileManager.h"

namespace Brickview::LDrawCore
{

  void Init(const std::filesystem::path& legoPartDataDirectory)
  {
    LDrawFileManager::Init(legoPartDataDirectory);
    LDrawCommandManager::Init();
  }

  void Shutdown()
  {
    LDrawFileManager::Shutdown();
    LDrawCommandManager::Shutdown();
  }

}
