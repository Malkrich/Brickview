#include "Pch.h"
#include "BrickviewCore.h"

#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer.h"

#include "Vendors/LDraw/LDrawCore.h"

namespace Brickview::BrickviewCore
{

	void init()
	{
		RenderCommand::init();
		Renderer::init();
		LDrawCore::init();
	}

	void shutdown()
	{
		RenderCommand::shutdown();
		Renderer::shutdown();
		LDrawCore::shutdown();
	}

}