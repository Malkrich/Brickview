#pragma once

#include "Renderer/Renderer/Renderer.h"

namespace Brickview
{

	class LightingRenderer : public Renderer
	{
	public:
		LightingRenderer();

		virtual void submitMesh(const Mesh& mesh) override;
		virtual void submitLight(const Light& light) override;
	};

}