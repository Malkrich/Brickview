#pragma once

#include "Renderer/CubeMap.h"

namespace Brickview
{

	class OpenGLCubeMap : public CubeMap
	{
	public:
		OpenGLCubeMap();

	private:
		void invalidate();

	private:
		uint32_t m_textureID = 0;
	};

}