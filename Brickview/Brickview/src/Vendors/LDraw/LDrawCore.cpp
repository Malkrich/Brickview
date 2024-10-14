#include "Pch.h"
#include "LDrawCore.h"

#include "LegoMeshLoader.h"
#include "LDrawFileManager.h"
#include "LDrawCommandManager.h"

namespace Brickview::LDrawCore
{

	void init()
	{
		LegoMeshLoader::init();
		LDrawFileManager::init();
		LDrawCommandManager::init();
	}

	void shutdown()
	{
		LegoMeshLoader::shutdown();
		LDrawFileManager::shutdown();
		LDrawCommandManager::shutdown();
	}

}