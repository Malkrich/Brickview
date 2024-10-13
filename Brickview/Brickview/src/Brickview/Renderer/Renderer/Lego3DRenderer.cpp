#include "Pch.h"
#include "Lego3DRenderer.h"
#include "RenderCommand.h"
#include "SolidRenderSystem.h"

namespace Brickview
{

	struct Lego3DRendererData
	{
		Scope<RenderSystem> RenderSystem = nullptr;
		Ref<ShaderLibrary> ShaderLibrary = nullptr;
	};

	static Lego3DRendererData* s_lego3DRendererData;

	void Lego3DRenderer::init()
	{
		RenderCommand::init();

		s_lego3DRendererData = new Lego3DRendererData();

		s_lego3DRendererData->ShaderLibrary = createRef<ShaderLibrary>();
		
		// For solid renderer
		s_lego3DRendererData->ShaderLibrary->load("data/Shaders/Solid.glsl");
		// For rendered renderer
		s_lego3DRendererData->ShaderLibrary->load("data/Shaders/LegoPiece.glsl");
		s_lego3DRendererData->ShaderLibrary->load("data/Shaders/Light.glsl");

		s_lego3DRendererData->RenderSystem = createScope<SolidRenderSystem>(s_lego3DRendererData->ShaderLibrary);
	}

	void Lego3DRenderer::shutdown()
	{
		delete s_lego3DRendererData;
		s_lego3DRendererData  = nullptr;
	}

	const Ref<ShaderLibrary>& Lego3DRenderer::getShaderLibrary()
	{
		BV_ASSERT(s_lego3DRendererData, "Lego3DRenderer has not been initialized!");
		return s_lego3DRendererData->ShaderLibrary;
	}

	void Lego3DRenderer::begin(const Camera& camera, const Light& light)
	{
		RenderCommand::setClearColor(0.2f, 0.2f, 0.2f);
		RenderCommand::clear();

		s_lego3DRendererData->RenderSystem->begin(camera, light);
	}

	void Lego3DRenderer::end()
	{
		s_lego3DRendererData->RenderSystem->end();
	}

	void Lego3DRenderer::drawMesh(const Ref<GpuMesh>& mesh, const Material& material, const glm::mat4& transform)
	{
		s_lego3DRendererData->RenderSystem->drawMesh(mesh, material, transform);
	}

	void Lego3DRenderer::drawLegoPart(const LegoPartComponent& legoPart, const glm::mat4& transform)
	{
		s_lego3DRendererData->RenderSystem->drawLegoPart(legoPart, transform);
	}

	const RenderStatistics& Lego3DRenderer::getRenderStatistics()
	{
		return s_lego3DRendererData->RenderSystem->getRenderStatistics();
	}

}