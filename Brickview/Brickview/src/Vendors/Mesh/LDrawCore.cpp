#include "Pch.h"
#include "LDrawCore.h"

#include "LDrawFileManager.h"
#include "LDrawCommandManager.h"

namespace Brickview::LDrawCore
{

	void init()
	{
		LDrawFileManager::init();
		LDrawCommandManager::init();
	}

	void shutdown()
	{
		LDrawFileManager::shutdown();
		LDrawCommandManager::shutdown();
	}

}