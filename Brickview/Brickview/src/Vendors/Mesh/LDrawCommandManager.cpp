#include "Pch.h"
#include "LDrawCommandManager.h"

#include "LDrawReader.h"

namespace Brickview
{

	struct LDrawCommandManagerData
	{
		std::unordered_map<std::string, LDrawCommandExtension> CommandExtensionLookup;
		std::unordered_map<std::string, LDrawCommandArgument> CommandArgumentLookup;

		using CommandExtensionCallbackFnc = std::function<void(const std::vector<LDrawCommandArgument>&, LDrawReader::States&)>;
		std::unordered_map<LDrawCommandExtension, CommandExtensionCallbackFnc> CommandExtensionCallbacks;
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

		s_ldrawCommandManagerData->CommandExtensionCallbacks[LDrawCommandExtension::BFC] = &onBFCCommandExecution;
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

	void LDrawCommandManager::onBFCCommandExecution(const std::vector<LDrawCommandArgument>& args, LDrawReader::States& readerStates)
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

	void LDrawCommandManager::executeCommand(LDrawCommandExtension extension, const std::vector<LDrawCommandArgument>& args, LDrawReader::States& readerStates)
	{
		if (!s_ldrawCommandManagerData->CommandExtensionCallbacks.contains(extension))
			return;

		s_ldrawCommandManagerData->CommandExtensionCallbacks.at(extension)(args, readerStates);
	}

}