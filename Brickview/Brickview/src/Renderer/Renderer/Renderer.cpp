#include "Pch.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "RenderedRenderer.h"
#include "SolidRenderer.h"

namespace Brickview
{

	struct RendererData
	{
		RendererType RendererType = RendererType::Solid;
	};

	static RendererData* s_rendererData;

	void Renderer::init()
	{
		RenderCommand::init();
		RenderedRenderer::init();

		s_rendererData = new RendererData();
	}

	void Renderer::shutdown()
	{
		RenderedRenderer::shutdown();

		delete s_rendererData;
		s_rendererData = nullptr;
	}

	void Renderer::setRenderType(RendererType type)
	{
		s_rendererData->RendererType = type;
	}

	void Renderer::submitMesh(const std::shared_ptr<Mesh>& mesh, const Material& material, const glm::mat4& transform)
	{
		switch (s_rendererData->RendererType)
		{
			case RendererType::Solid:
				SolidRenderer::submitMesh(mesh, transform);
				break;
			case RendererType::Rendered:
				RenderedRenderer::submitMesh(mesh, material, transform);
				break;
		}
	}

}