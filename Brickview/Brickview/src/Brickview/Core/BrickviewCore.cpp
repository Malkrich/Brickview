#include "Pch.h"
#include "BrickviewCore.h"

#include "Renderer/Renderer/Lego3DRenderer.h"
#include "Vendors/Mesh/LDrawFileManager.h"

namespace Brickview::BrickviewCore
{

	void init()
	{
		Lego3DRenderer::init();
		LDrawFileManager::init();
	}

	void shutdown()
	{
		Lego3DRenderer::shutdown();
		LDrawFileManager::shutdown();
	}

}