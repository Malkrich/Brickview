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

	void Renderer::renderMeshInstances(Ref<Shader> shader, const Ref<GpuMesh>& mesh, const glm::mat4* transformData, size_t instanceCount)
	{
		// Transform buffer
		uint32_t transformBufferSize = instanceCount * sizeof(glm::mat4);

		Ref<VertexBuffer> meshTransformBuffer = VertexBuffer::create(transformBufferSize, transformData);
		// Note: as a GpuMesh is passed, we assume that the transform buffer will be at the 3rd position (bind = 2)
		// Maybe needs to be revisited
		Layout meshTransformLayout = {
			{ 2, "a_transform", BufferElementType::Mat4 }
		};
		meshTransformBuffer->setBufferLayout(meshTransformLayout);

		Ref<VertexArray> vao = VertexArray::create();
		vao->addVertexBuffer(mesh->getGeometryVertexBuffer());
		vao->setIndexBuffer(mesh->getGeometryIndexBuffer());
		vao->addVertexBuffer(meshTransformBuffer);

		shader->bind();

		RenderCommand::drawInstances(vao, instanceCount);
	}

}