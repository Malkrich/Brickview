#include "Pch.h"
#include "BrickviewCore.h"

#include "Renderer/Renderer/Lego3DRenderer.h"
#include "Mesh/LegoMeshLoader.h"

namespace Brickview::BrickviewCore
{

	void init()
	{
		Lego3DRenderer::init();
		LegoMeshLoader::init();
	}

	void shutdown()
	{
		Lego3DRenderer::shutdown();
		LegoMeshLoader::shutdown();
	}

}