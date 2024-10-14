#pragma once

#include <Brickview.h>
#include <Core/EntryPoint.h>

#include "ApplicationLayer.h"

namespace Brickview
{

	class BrickviewApp : public Application
	{
	public:
		BrickviewApp()
			: Application()
		{
			pushLayer(new ApplicationLayer());
		}
	};

	Application* createApplication()
	{
		return new BrickviewApp();
	}

}