#include "Pch.h"
#include "BrickviewCore.h"

#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer/Lego3DRenderer.h"
#include "Vendors/Mesh/LDrawCore.h"

namespace Brickview::BrickviewCore
{

	void init()
	{
		RenderCommand::init();
		Lego3DRenderer::init();
		LDrawCore::init();
	}

	void shutdown()
	{
		RenderCommand::shutdown();
		Lego3DRenderer::shutdown();
		LDrawCore::shutdown();
	}

}