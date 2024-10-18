#include "Pch.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Buffer.h"

#include "Vendors/OpenGL/OpenGLError.h"

namespace Brickview
{

	struct LineVertex
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };

		LineVertex() = default;
		LineVertex(const glm::vec3& position, const glm::vec3 color)
			: Position(position), Color(color) {}
	};

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
		s_rendererData->ShaderLibrary->load("data/Shaders/Line.glsl");
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

		Ref<VertexArray> instanceDrawCallVertexArray = VertexArray::create();
		instanceDrawCallVertexArray->addVertexBuffer(mesh->getGeometryVertexBuffer());
		instanceDrawCallVertexArray->addVertexBuffer(instanceElementsBuffer);
		instanceDrawCallVertexArray->setIndexBuffer(mesh->getGeometryIndexBuffer());

		shader->bind();
		instanceDrawCallVertexArray->bind();
		RenderCommand::drawInstances(instanceDrawCallVertexArray, instanceCount);
		instanceDrawCallVertexArray->unbind();
	}

	void Renderer::renderLines(const std::vector<Line>& lines, const glm::vec3& color, float lineWidth)
	{
		std::vector<glm::vec3> colors(lines.size());
		std::fill(colors.begin(), colors.end(), color);
		renderLines(lines, colors, lineWidth);
	}

	void Renderer::renderLines(const std::vector<Line>& lines, const std::vector<glm::vec3>& colors, float lineWidth)
	{
		BV_ASSERT(lines.size() == colors.size(), "Lines and color data must be the same length!");

		std::vector<LineVertex> lineVertices(lines.size() * 2);
		for (uint32_t i = 0; i < lines.size(); i++)
		{
			const Line& line = lines[i];
			const glm::vec3& color = colors[i];
			lineVertices[2 * i + 0] = LineVertex(line.P0, color);
			lineVertices[2 * i + 1] = LineVertex(line.P1, color);
		}

		uint32_t vertexCount = lineVertices.size();
		Ref<VertexBuffer> linesVertexBuffer = VertexBuffer::create(vertexCount * sizeof(LineVertex), lineVertices.data());
		Layout lineVertexLayout = {
			{ 0, "a_position", BufferElementType::Float3 },
			{ 1, "a_color", BufferElementType::Float3 }
		};
		linesVertexBuffer->setBufferLayout(lineVertexLayout);

		Ref<VertexArray> linesVertexArray = VertexArray::create();
		linesVertexArray->addVertexBuffer(linesVertexBuffer);

		Ref<Shader> lineShader = s_rendererData->ShaderLibrary->get("Line");
		lineShader->bind();

		linesVertexArray->bind();
		RenderCommand::setLineWidth(lineWidth);
		RenderCommand::drawLines(linesVertexArray, vertexCount);
		linesVertexArray->unbind();
	}

}