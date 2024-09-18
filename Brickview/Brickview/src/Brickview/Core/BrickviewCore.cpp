#include "Pch.h"
#include "BrickviewCore.h"

#include "Renderer/Renderer/Lego3DRenderer.h"
#include "Vendors/Mesh/LDrawCore.h"

namespace Brickview::BrickviewCore
{

	void init()
	{
		Lego3DRenderer::init();
		LDrawCore::init();
	}

	void shutdown()
	{
		Lego3DRenderer::shutdown();
		LDrawCore::shutdown();
	}

}