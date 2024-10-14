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
		CW, CCW,
		InvertNext
	};

	class LDrawCommandManager
	{
	public:
		static void init();
		static void shutdown();

		// Command Extensions
		static bool isCommand(const std::string& rawExtension);
		static LDrawCommandExtension getCommandExtension(const std::string& rawExtension);
		// Command arguments
		static LDrawCommandArgument getCommandArgument(const std::string& rawArgument);

		static void executeCommand(LDrawCommandExtension extension, const std::vector<LDrawCommandArgument>& args, LDrawReader::States& readerStates);

	private:
		// Command callbacks
		static void onBFCCommandExecution(const std::vector<LDrawCommandArgument>& args, LDrawReader::States& readerStates);
	};

}