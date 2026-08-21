#pragma once

#include "LDrawReader.h"

namespace Brickview
{

  enum class LDrawCommandExtension
  {
    None = 0,
    BFC
  };

  enum class LDrawCommandArgument
  {
    None = 0,
    Certify,
    CW,
    CCW,
    InvertNext
  };

  class LDrawCommandManager
  {
  public:
    static void Init();
    static void Shutdown();

    // Command Extensions
    static bool IsCommand(const std::string& rawExtension);
    static LDrawCommandExtension GetCommandExtension(const std::string& rawExtension);
    // Command arguments
    static LDrawCommandArgument GetCommandArgument(const std::string& rawArgument);

    static void ExecuteCommand(LDrawCommandExtension extension, const std::vector<LDrawCommandArgument>& args,
      LDrawReader::States& readerStates);

  private:
    // Command callbacks
    static void OnBFCCommandExecution(const std::vector<LDrawCommandArgument>& args, LDrawReader::States& readerStates);
  };

}
