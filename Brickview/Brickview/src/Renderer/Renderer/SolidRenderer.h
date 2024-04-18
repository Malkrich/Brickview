#pragma once

#include "Renderer/Renderer/Renderer.h"

namespace Brickview
{

	class SolidRenderer : public Renderer
	{
	public:
		SolidRenderer();

		virtual void submitMesh(const Mesh& mesh) override;
		virtual void submitLight(const Light& light) override;
	};

}