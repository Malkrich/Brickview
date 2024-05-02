#include "Pch.h"
#include "SolidRenderer.h"

#include "Renderer/Shader/Shader.h"

namespace Brickview
{

	struct SolidRendererData
	{
		std::shared_ptr<Shader> shader;
	};

	static SolidRenderer* s_rendererData;

	void SolidRenderer::init()
	{

	}

	void SolidRenderer::submitMesh(const Mesh& mesh)
	{
	}

	void SolidRenderer::submitLight(const Light& light)
	{		
	}

}