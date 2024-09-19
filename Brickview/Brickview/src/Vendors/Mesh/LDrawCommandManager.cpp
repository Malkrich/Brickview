#include "Pch.h"
#include "LDrawCommandManager.h"

namespace Brickview
{

	struct LDrawCommandManagerData
	{
		std::unordered_map<std::string, LDrawCommandExtension> CommandExtensionLookup;
		std::unordered_map<std::string, LDrawCommandArgument> CommandArgumentLookup;
	};

	static LDrawCommandManagerData* s_ldrawCommandManagerData = nullptr;

	void LDrawCommandManager::init()
	{
		s_ldrawCommandManagerData = new LDrawCommandManagerData();
		// Command extensions
		s_ldrawCommandManagerData->CommandExtensionLookup["BFC"]       = LDrawCommandExtension::BFC;
		// Command arguments
		s_ldrawCommandManagerData->CommandArgumentLookup["CERTIFY"]    = LDrawCommandArgument::Certify;
		s_ldrawCommandManagerData->CommandArgumentLookup["CW"]         = LDrawCommandArgument::CW;
		s_ldrawCommandManagerData->CommandArgumentLookup["CCW"]        = LDrawCommandArgument::CCW;
		s_ldrawCommandManagerData->CommandArgumentLookup["INVERTNEXT"] = LDrawCommandArgument::InvertNext;
	}

	void LDrawCommandManager::shutdown()
	{
		delete s_ldrawCommandManagerData;
		s_ldrawCommandManagerData = nullptr;
	}

	bool LDrawCommandManager::isCommand(const std::string& rawExtension)
	{
		// TODO (backward compatibility):
		// needs to check whether rawExtension contains "!" before the command extension
		return s_ldrawCommandManagerData->CommandExtensionLookup.contains(rawExtension);
	}

	LDrawCommandExtension LDrawCommandManager::getCommandExtension(const std::string& rawExtension)
	{
		if (s_ldrawCommandManagerData->CommandExtensionLookup.contains(rawExtension))
			return s_ldrawCommandManagerData->CommandExtensionLookup.at(rawExtension);

		return LDrawCommandExtension::None;
	}

	LDrawCommandArgument LDrawCommandManager::getCommandArgument(const std::string& rawArgument)
	{
		if (s_ldrawCommandManagerData->CommandArgumentLookup.contains(rawArgument))
			return s_ldrawCommandManagerData->CommandArgumentLookup.at(rawArgument);
		else
			return LDrawCommandArgument::None;
	}

	void LDrawCommandManager::executeCommand(LDrawCommandExtension extension, const std::vector<LDrawCommandArgument>& args, LoadingSettings& settings)
	{

	}

}