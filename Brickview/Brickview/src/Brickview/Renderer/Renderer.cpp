#include "Pch.h"
#include "Renderer.h"

#include "Core/Buffer.h"
#include "RendererCore.h"
#include "RenderCommand.h"
#include "GpuBuffer.h"
#include "UniformBuffer.h"

#include "Vendors/OpenGL/OpenGLError.h"

#define BV_MAX_LIGHTS_SUPPORTED 10

namespace Brickview
{

	struct LightVertex
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };

		LightVertex() = default;
		LightVertex(const glm::vec3& position)
			: Position(position) {}
	};

	struct PointLightInstance
	{
		int LightIndex = 0;
		int EntityID = -1;

		PointLightInstance() = default;
		PointLightInstance(int lightIndex, int entityID)
			: LightIndex(lightIndex)
			, EntityID(entityID) {}
	};

	struct GpuPointLightStruct
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		BV_GPU_INTERNAL_PADDING(1);
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		BV_GPU_INTERNAL_PADDING(1);

		GpuPointLightStruct() = default;
		GpuPointLightStruct(const PointLight& light)
			: Position(light.Position)
			, Color(light.Color)
		{}
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

		// Camera
		Ref<UniformBuffer> CameraUbo = nullptr;

		// Point Lights
		int PointLightsCount = 0;
		Ref<UniformBuffer> PointLightsUbo = nullptr;
		Ref<VertexBuffer> PointLightInstancesVbo = nullptr;
		Layout PointLightInstancesBufferVboLayout;
		std::vector<PointLightInstance> PointLightInstancesData;
		Ref<GpuMesh> LightMesh = nullptr;
	};

	static RendererData* s_rendererData;

	void Renderer::init()
	{
		s_rendererData = new RendererData();

		// Shaders
		s_rendererData->ShaderLibrary = createRef<ShaderLibrary>();
		std::filesystem::path shaderBaseDir = "data/Shaders/";
		//s_rendererData->ShaderLibrary->load(shaderBaseDir / "SolidMesh.glsl");
		//s_rendererData->ShaderLibrary->load(shaderBaseDir / "PhongMesh.glsl");
		//s_rendererData->ShaderLibrary->load(shaderBaseDir / "PBRMesh.glsl");
		s_rendererData->ShaderLibrary->load(shaderBaseDir / "Light.glsl");
		s_rendererData->ShaderLibrary->load(shaderBaseDir / "Line.glsl");

		// Camera
		UniformBufferSpecifications cameraDataSpecs;
		cameraDataSpecs.BlockName = "CameraData";
		cameraDataSpecs.BindingPoint = 0;
		s_rendererData->CameraUbo = UniformBuffer::create(cameraDataSpecs, sizeof(CameraData));

		// Upload light cube mesh to GPU
		std::vector<LightVertex> lightMeshVertices;
		std::vector<TriangleFace> lightMeshIndices;
		{
			Ref<Mesh> lightSourceMesh = Mesh::load("data/Meshes/Cube.obj");
			lightSourceMesh->scale(0.01f);

			// Vertices
			const auto& sourceMeshVertices = lightSourceMesh->getVertices();
			lightMeshVertices.reserve(sourceMeshVertices.size());
			for (const auto& sourceVertex : sourceMeshVertices)
				lightMeshVertices.emplace_back(sourceVertex.Position);

			// Indices
			lightMeshIndices = lightSourceMesh->getConnectivities();
		}

		Layout pointLightMeshLayout = {
			{ 0, "a_position", BufferElementType::Float3 }
		};
		Layout pointLightInstancesLayout = {
			{ 1, "a_lightInstanceIndex", BufferElementType::Int, 1 },
			{ 2, "a_lightInstanceEntityID", BufferElementType::Int, 1 }
		};

		s_rendererData->LightMesh = createRef<GpuMesh>(
			lightMeshVertices.size() * sizeof(LightVertex), lightMeshVertices.data(), 
			pointLightMeshLayout,
			lightMeshIndices.size() * sizeof(TriangleFace), lightMeshIndices.data());
		s_rendererData->PointLightInstancesVbo = VertexBuffer::create(sizeof(PointLightInstance));
		s_rendererData->PointLightInstancesVbo->setBufferLayout(pointLightInstancesLayout);

		UniformBufferSpecifications pointLightsDataSpecs;
		pointLightsDataSpecs.BlockName = "PointLightsData";
		pointLightsDataSpecs.BindingPoint = 1;
		s_rendererData->PointLightsUbo = UniformBuffer::create(pointLightsDataSpecs, 4 * sizeof(s_rendererData->PointLightsCount) + BV_MAX_LIGHTS_SUPPORTED * sizeof(GpuPointLightStruct));
	}

	void Renderer::shutdown()
	{
		delete s_rendererData;
		s_rendererData = nullptr;
	}

	void Renderer::begin(const CameraData& cameraData, const std::vector<PointLight>& pointLights, const std::vector<int>& pLIDs)
	{
		BV_ASSERT(pointLights.size() == pLIDs.size(), "Point light object and ID vectors do not have the same size!");
		BV_ASSERT(pointLights.size() <= BV_MAX_LIGHTS_SUPPORTED, "Renderer does not support more than 10 lights!");

		// Camera
		s_rendererData->CameraUbo->setData(&cameraData);

		// Point Lights
		s_rendererData->PointLightsCount = pointLights.size();
		std::array<GpuPointLightStruct, BV_MAX_LIGHTS_SUPPORTED> gpuPointLights;
		s_rendererData->PointLightInstancesData.resize(s_rendererData->PointLightsCount);
		for (int i = 0; i < s_rendererData->PointLightsCount; i++)
		{
			gpuPointLights[i] = GpuPointLightStruct(pointLights[i]);
			s_rendererData->PointLightInstancesData[i] = { i, pLIDs[i] };
		}
		// Computing sizes
		uint32_t pointLightsArraySize = BV_MAX_LIGHTS_SUPPORTED * sizeof(GpuPointLightStruct);
		uint32_t bufferSize = 4 * sizeof(s_rendererData->PointLightsCount) + pointLightsArraySize;
		// Copying point lights data
		Buffer pointLightUboData(bufferSize);
		memcpy(pointLightUboData.Data, &s_rendererData->PointLightsCount, sizeof(s_rendererData->PointLightsCount));
		memcpy(pointLightUboData.Data + 4 * sizeof(s_rendererData->PointLightsCount), gpuPointLights.data(), pointLightsArraySize);
		s_rendererData->PointLightsUbo->setData(pointLightUboData.Data);
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

	void Renderer::renderPointLights()
	{
		if (s_rendererData->PointLightsCount == 0)
			return;

		s_rendererData->PointLightInstancesVbo->setData(
			s_rendererData->PointLightsCount * sizeof(PointLightInstance), 
			s_rendererData->PointLightInstancesData.data());

		// Light mesh
		Ref<VertexArray> lightVertexArray = VertexArray::create();
		lightVertexArray->addVertexBuffer(s_rendererData->LightMesh->getGeometryVertexBuffer());
		lightVertexArray->addVertexBuffer(s_rendererData->PointLightInstancesVbo);
		lightVertexArray->setIndexBuffer(s_rendererData->LightMesh->getGeometryIndexBuffer());

		Ref<Shader> lightShader = s_rendererData->ShaderLibrary->get("Light");

		lightShader->bind();
		RenderCommand::drawInstances(lightVertexArray, s_rendererData->PointLightsCount);
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