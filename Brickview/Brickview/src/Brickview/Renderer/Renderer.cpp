#include "Pch.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Buffer.h"

#include "Vendors/OpenGL/OpenGLError.h"

namespace Brickview
{

	struct RendererData
	{
		Ref<ShaderLibrary> ShaderLibrary = nullptr;
	};

	static RendererData* s_rendererData;

	void Renderer::init()
	{
		s_rendererData = new RendererData();

		s_rendererData->ShaderLibrary = createRef<ShaderLibrary>();
		
		// For solid renderer
		s_rendererData->ShaderLibrary->load("data/Shaders/Solid.glsl");
		// For rendered renderer
		s_rendererData->ShaderLibrary->load("data/Shaders/LegoPiece.glsl");
		s_rendererData->ShaderLibrary->load("data/Shaders/Light.glsl");
	}

	void Renderer::shutdown()
	{
		delete s_rendererData;
		s_rendererData = nullptr;
	}

	const Ref<ShaderLibrary>& Renderer::getShaderLibrary()
	{
		BV_ASSERT(s_rendererData, "Renderer class is not initialized!");
		return s_rendererData->ShaderLibrary;
	}

	void Renderer::renderMeshInstances(Ref<Shader> shader, const Ref<GpuMesh>& mesh, 
		const void* instanceBufferData, const Layout& instanceBufferLayout, uint32_t instanceBufferSize, size_t instanceCount)
	{
		Ref<VertexBuffer> instanceElementsBuffer = VertexBuffer::create(instanceCount * instanceBufferSize, instanceBufferData);
		instanceElementsBuffer->setBufferLayout(instanceBufferLayout);

		Ref<VertexArray> vertexArray = VertexArray::create();
		vertexArray->addVertexBuffer(mesh->getGeometryVertexBuffer());
		vertexArray->addVertexBuffer(instanceElementsBuffer);
		vertexArray->setIndexBuffer(mesh->getGeometryIndexBuffer());

		shader->bind();

		RenderCommand::drawInstances(vertexArray, instanceCount);

		vertexArray->unbind();
	}

}