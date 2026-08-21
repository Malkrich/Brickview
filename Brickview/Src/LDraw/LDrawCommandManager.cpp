

#include "LDrawCommandManager.h"

#include "LDrawReader.h"

namespace Brickview
{

  struct LDrawCommandManagerData
  {
    std::unordered_map<std::string, LDrawCommandExtension> CommandExtensionLookup;
    std::unordered_map<std::string, LDrawCommandArgument> CommandArgumentLookup;

    using CommandExtensionCallbackFnc =
      std::function<void(const std::vector<LDrawCommandArgument>&, LDrawReader::States&)>;
    std::unordered_map<LDrawCommandExtension, CommandExtensionCallbackFnc> CommandExtensionCallbacks;
  };

  static LDrawCommandManagerData* s_LdrawCommandManagerData = nullptr;

  void LDrawCommandManager::Init()
  {
    s_LdrawCommandManagerData = new LDrawCommandManagerData();
    // Command extensions
    s_LdrawCommandManagerData->CommandExtensionLookup["BFC"] = LDrawCommandExtension::BFC;
    // Command arguments
    s_LdrawCommandManagerData->CommandArgumentLookup["CERTIFY"] = LDrawCommandArgument::Certify;
    s_LdrawCommandManagerData->CommandArgumentLookup["CW"] = LDrawCommandArgument::CW;
    s_LdrawCommandManagerData->CommandArgumentLookup["CCW"] = LDrawCommandArgument::CCW;
    s_LdrawCommandManagerData->CommandArgumentLookup["INVERTNEXT"] = LDrawCommandArgument::InvertNext;

    s_LdrawCommandManagerData->CommandExtensionCallbacks[LDrawCommandExtension::BFC] = &OnBFCCommandExecution;
  }

  void LDrawCommandManager::Shutdown()
  {
    delete s_LdrawCommandManagerData;
    s_LdrawCommandManagerData = nullptr;
  }

  bool LDrawCommandManager::IsCommand(const std::string& rawExtension)
  {
    // TODO (backward compatibility):
    // needs to check whether rawExtension contains "!" before the command extension
    return s_LdrawCommandManagerData->CommandExtensionLookup.contains(rawExtension);
  }

  LDrawCommandExtension LDrawCommandManager::GetCommandExtension(const std::string& rawExtension)
  {
    if (s_LdrawCommandManagerData->CommandExtensionLookup.contains(rawExtension))
      return s_LdrawCommandManagerData->CommandExtensionLookup.at(rawExtension);

    return LDrawCommandExtension::None;
  }

  LDrawCommandArgument LDrawCommandManager::GetCommandArgument(const std::string& rawArgument)
  {
    if (s_LdrawCommandManagerData->CommandArgumentLookup.contains(rawArgument))
      return s_LdrawCommandManagerData->CommandArgumentLookup.at(rawArgument);
    else
      return LDrawCommandArgument::None;
  }

  void LDrawCommandManager::OnBFCCommandExecution(
    const std::vector<LDrawCommandArgument>& args, LDrawReader::States& readerStates)
  {
    // Single argument case
    if (args.size() == 1)
    {
      auto argument = args[0];
      if (argument == LDrawCommandArgument::Certify)
        readerStates.Winding = LDrawGeometryWinding::CCW;
      else if (argument == LDrawCommandArgument::InvertNext)
        readerStates.Inverted = !readerStates.Inverted;
      else if (argument == LDrawCommandArgument::CW)
        readerStates.Winding = LDrawGeometryWinding::CW;
      else if (argument == LDrawCommandArgument::CCW)
        readerStates.Winding = LDrawGeometryWinding::CCW;
      return;
    }

    // Two arguments
    if (args[0] == LDrawCommandArgument::Certify)
    {
      auto secondArgument = args[1];
      if (secondArgument == LDrawCommandArgument::CW)
        readerStates.Winding = LDrawGeometryWinding::CW;
      else if (secondArgument == LDrawCommandArgument::CCW)
        readerStates.Winding = LDrawGeometryWinding::CCW;
    }
  }

  void LDrawCommandManager::ExecuteCommand(
    LDrawCommandExtension extension, const std::vector<LDrawCommandArgument>& args, LDrawReader::States& readerStates)
  {
    if (!s_LdrawCommandManagerData->CommandExtensionCallbacks.contains(extension))
      return;

    s_LdrawCommandManagerData->CommandExtensionCallbacks.at(extension)(args, readerStates);
  }

}
