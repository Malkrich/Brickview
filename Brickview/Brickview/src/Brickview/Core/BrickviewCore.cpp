#include "Pch.h"
#include "BrickviewCore.h"

#include "Renderer/RendererAPI.h"
#include "Renderer/Renderer/Lego3DRenderer.h"
#include "Vendors/Mesh/LDrawCore.h"

namespace Brickview::BrickviewCore
{

	void init()
	{
		RendererAPI::init();
		Lego3DRenderer::init();
		LDrawCore::init();
	}

	void shutdown()
	{
		RendererAPI::shutdown();
		Lego3DRenderer::shutdown();
		LDrawCore::shutdown();
	}

}