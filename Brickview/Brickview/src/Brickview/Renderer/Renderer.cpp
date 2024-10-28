#include "Pch.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Buffer.h"
#include "UniformBuffer.h"

#include "Vendors/OpenGL/OpenGLError.h"

namespace Brickview
{

	struct LightVertex
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };

		LightVertex() = default;
		LightVertex(const glm::vec3& position)
			: Position(position) {}
	};

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
		// Shaders
		Ref<ShaderLibrary> ShaderLibrary = nullptr;

		Ref<GpuMesh> LightMesh = nullptr;
	};

	static RendererData* s_rendererData;

	void Renderer::init()
	{
		s_rendererData = new RendererData();

		s_rendererData->ShaderLibrary = createRef<ShaderLibrary>();
		
		// Meshes
		s_rendererData->ShaderLibrary->load("data/Shaders/SolidMesh.glsl");
		s_rendererData->ShaderLibrary->load("data/Shaders/LightedMesh.glsl");
		// Lights
		s_rendererData->ShaderLibrary->load("data/Shaders/Light.glsl");
		// Lines
		s_rendererData->ShaderLibrary->load("data/Shaders/Line.glsl");


		// Upload light cube mesh to GPU
		std::vector<LightVertex> lightMeshVertices;
		std::vector<TriangleFace> lightMeshIndices;
		{
			Ref<Mesh> lightSourceMesh = Mesh::load("data/Meshes/Cube.obj");
			lightSourceMesh->scale(0.1f);

			// Vertices
			const auto& sourceMeshVertices = lightSourceMesh->getVertices();
			lightMeshVertices.reserve(sourceMeshVertices.size());
			for (const auto& sourceVertex : sourceMeshVertices)
				lightMeshVertices.emplace_back(sourceVertex.Position);

			// Indices
			lightMeshIndices = lightSourceMesh->getConnectivities();
		}

		Layout lightMeshLayout = {
			{ 0, "a_position", BufferElementType::Float3 }
		};

		s_rendererData->LightMesh = createRef<GpuMesh>(lightMeshVertices.size() * sizeof(LightVertex), lightMeshVertices.data(), lightMeshLayout,
			lightMeshIndices.size() * sizeof(TriangleFace), lightMeshIndices.data());
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
		RenderCommand::drawInstances(instanceDrawCallVertexArray, instanceCount);
		instanceDrawCallVertexArray->unbind();
	}

	void Renderer::renderLight(const Light& light, int entityID)
	{
		// Light draw data
		UniformBufferSpecifications lightDrawDataUboSpecs;
		lightDrawDataUboSpecs.BlockName = "LightDrawData";
		lightDrawDataUboSpecs.BindingPoint = 2;
		lightDrawDataUboSpecs.Layout = {
			UniformBufferElementType::Float3,
			UniformBufferElementType::Float3,
			UniformBufferElementType::Int,
		};
		Ref<UniformBuffer> lightDrawDataUbo = UniformBuffer::create(lightDrawDataUboSpecs);
		lightDrawDataUbo->setElement(0, &light.Position);
		lightDrawDataUbo->setElement(1, &light.Color);
		lightDrawDataUbo->setElement(2, &entityID);

		// Light mesh
		Ref<VertexArray> lightVertexArray = VertexArray::create();
		lightVertexArray->addVertexBuffer(s_rendererData->LightMesh->getGeometryVertexBuffer());
		lightVertexArray->setIndexBuffer(s_rendererData->LightMesh->getGeometryIndexBuffer());

		Ref<Shader> lightShader = s_rendererData->ShaderLibrary->get("Light");

		lightShader->bind();
		RenderCommand::drawIndices(lightVertexArray);
		lightVertexArray->unbind();
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

		RenderCommand::setLineWidth(lineWidth);
		RenderCommand::drawLines(linesVertexArray, vertexCount);
		linesVertexArray->unbind();
	}

}