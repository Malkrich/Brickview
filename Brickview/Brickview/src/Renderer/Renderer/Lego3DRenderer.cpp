#include "Pch.h"
#include "Lego3DRenderer.h"
#include "RenderCommand.h"
#include "RenderedRenderer.h"
#include "SolidRenderer.h"

namespace Brickview
{

	struct Lego3DRendererData
	{
		RenderType RenderType = RenderType::Rendered;
		bool drawLights = true;

		Scope<RendererBase> Renderer = nullptr;
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

		s_lego3DRendererData->Renderer = rendererFromType(s_lego3DRendererData->RenderType);
	}

	void Lego3DRenderer::shutdown()
	{
		delete s_lego3DRendererData;
		s_lego3DRendererData  = nullptr;
	}

	Scope<RendererBase> Lego3DRenderer::rendererFromType(RenderType type)
	{
		BV_ASSERT(s_lego3DRendererData, "Lego3DRenderer has not been initialized!");

		switch (type)
		{
			case RenderType::Solid:    return createScope<SolidRenderer>(s_lego3DRendererData->ShaderLibrary);
			case RenderType::Rendered: return createScope<RenderedRenderer>(s_lego3DRendererData->ShaderLibrary);
		}

		BV_ASSERT(false, "Render Type not known!");
		return nullptr;
	}

	const Ref<ShaderLibrary>& Lego3DRenderer::getShaderLibrary()
	{
		BV_ASSERT(s_lego3DRendererData, "Lego3DRenderer has not been initialized!");
		return s_lego3DRendererData->ShaderLibrary;
	}

	const Ref<RenderSettings>& Lego3DRenderer::getRenderSettings()
	{
		BV_ASSERT(s_lego3DRendererData, "Lego3DRenderer has not been initialized!");
		return s_lego3DRendererData->Renderer->getRenderSettings();
	}

	bool Lego3DRenderer::isDrawingLights()
	{
		BV_ASSERT(s_lego3DRendererData, "Lego3DRenderer has not been initialized!");
		return s_lego3DRendererData->drawLights;
	}

	void Lego3DRenderer::drawLights(bool drawLights)
	{
		BV_ASSERT(s_lego3DRendererData, "Lego3DRenderer has not been initialized!");
		s_lego3DRendererData->drawLights = drawLights;
	}

	void Lego3DRenderer::setRenderType(RenderType type)
	{
		BV_ASSERT(s_lego3DRendererData, "Lego3DRenderer has not been initialized!");
		s_lego3DRendererData->RenderType = type;
		s_lego3DRendererData->Renderer   = rendererFromType(type);
	}

	void Lego3DRenderer::begin(const Camera& camera, const Light& light)
	{
		RenderCommand::setClearColor(0.2f, 0.2f, 0.2f);
		RenderCommand::clear();

		s_lego3DRendererData->Renderer->begin(camera, light);

		if (s_lego3DRendererData->drawLights)
			s_lego3DRendererData->Renderer->drawLights(light);
	}

	void Lego3DRenderer::end()
	{
		s_lego3DRendererData->Renderer->end();
	}

	void Lego3DRenderer::drawMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform)
	{
		if (mesh->isEmpty())
			return;

		s_lego3DRendererData->Renderer->drawMesh(mesh, material, transform);
	}

}