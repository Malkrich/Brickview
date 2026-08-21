

#include "LDrawCore.h"

#include "LDrawCommandManager.h"
#include "LDrawFileManager.h"

namespace Brickview::LDrawCore
{

  void Init()
  {
    LDrawFileManager::Init();
    LDrawCommandManager::Init();
  }

  void Shutdown()
  {
    LDrawFileManager::Shutdown();
    LDrawCommandManager::Shutdown();
  }

}
