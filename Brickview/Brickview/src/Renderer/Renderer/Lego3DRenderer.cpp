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

		// Has to be initialized after in order to access the shader library
		RenderedRenderer::init(
			s_lego3DRendererData->ShaderLibrary->get("LegoPiece"),
			s_lego3DRendererData->ShaderLibrary->get("Light"));
		SolidRenderer::init(s_lego3DRendererData->ShaderLibrary->get("Solid"));
	}

	void Lego3DRenderer::shutdown()
	{
		RenderedRenderer::shutdown();
		SolidRenderer::shutdown();

		delete s_lego3DRendererData;
		s_lego3DRendererData  = nullptr;
	}

	const Ref<ShaderLibrary>& Lego3DRenderer::getShaderLibrary()
	{
		BV_ASSERT(s_lego3DRendererData, "Lego3DRenderer has not been initialized !");
		return s_lego3DRendererData->ShaderLibrary;
	}

	bool Lego3DRenderer::isDrawingLights()
	{
		BV_ASSERT(s_lego3DRendererData, "Lego3DRenderer has not been initialized !");
		return s_lego3DRendererData->drawLights;
	}

	void Lego3DRenderer::drawLights(bool drawLights)
	{
		BV_ASSERT(s_lego3DRendererData, "Lego3DRenderer has not been initialized !");
		s_lego3DRendererData->drawLights = drawLights;
	}

	RenderType Lego3DRenderer::getRenderType()
	{
		BV_ASSERT(s_lego3DRendererData, "Lego3DRenderer has not been initialized !");
		return s_lego3DRendererData->RenderType;
	}

	void Lego3DRenderer::setRenderType(RenderType type)
	{
		BV_ASSERT(s_lego3DRendererData, "Lego3DRenderer has not been initialized !");
		s_lego3DRendererData->RenderType = type;
	}

	void Lego3DRenderer::begin(const Camera& camera, const Light& light)
	{
		RenderCommand::setClearColor(0.2f, 0.2f, 0.2f);
		RenderCommand::clear();

		switch (s_lego3DRendererData->RenderType)
		{
			case RenderType::Solid:
				SolidRenderer::begin(camera, light);
				break;
			case RenderType::Rendered:
				RenderedRenderer::begin(camera, light);
				break;
		}

		if (s_lego3DRendererData->drawLights)
		{
			switch (s_lego3DRendererData->RenderType)
			{
				case RenderType::Solid:
					return;
				case RenderType::Rendered:
					RenderedRenderer::submitLights();
					return;
			}
		}
	}

	void Lego3DRenderer::end()
	{
		switch (s_lego3DRendererData->RenderType)
		{
			case RenderType::Solid:
				SolidRenderer::end();
				return;
			case RenderType::Rendered:
				RenderedRenderer::end();
				return;
		}
	}

	void Lego3DRenderer::drawMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform)
	{
		switch (s_lego3DRendererData->RenderType)
		{
			case RenderType::Solid:
				SolidRenderer::submitMesh(mesh, transform);
				return;
			case RenderType::Rendered:
				RenderedRenderer::submitMesh(mesh, material, transform);
				return;
		}
	}

}