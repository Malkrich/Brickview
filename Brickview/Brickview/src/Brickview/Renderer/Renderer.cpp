#include "Pch.h"
#include "Renderer.h"

#include "RendererCore.h"
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
		glm::vec3 Position;
		BV_GPU_INTERNAL_PADDING(1);
		glm::vec3 Color;
		BV_GPU_INTERNAL_PADDING(1);

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

		// Lights
		int PointLightsCount = 0;
		Ref<UniformBuffer> PointLightsUbo = nullptr;
		Ref<VertexBuffer> PointLightInstancesVbo = nullptr;

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

		Layout lightMeshLayout = {
			{ 0, "a_position", BufferElementType::Float3 }
		};

		// TODO: s_rendererData->Light = createRef<GpuMesh>(lightMeshVertices, lightMeshLayout, lightMeshIndices);
		// Where lightMeshVertices and lightMeshIndices are buffers containing Size and Data attributes
		s_rendererData->LightMesh = createRef<GpuMesh>(lightMeshVertices.size() * sizeof(LightVertex), lightMeshVertices.data(), 
			lightMeshLayout, 
			lightMeshIndices.size() * sizeof(TriangleFace), lightMeshIndices.data());

		UniformBufferSpecifications pointLightsDataSpecs;
		pointLightsDataSpecs.BlockName = "PointLightsData";
		pointLightsDataSpecs.BindingPoint = 1;
		s_rendererData->PointLightsUbo = UniformBuffer::create(pointLightsDataSpecs, 10 * sizeof(GpuPointLightStruct) + sizeof(int));
	}

	void Renderer::shutdown()
	{
		delete s_rendererData;
		s_rendererData = nullptr;
	}

	void Renderer::begin(const CameraData& cameraData, const LightsData& lightsData)
	{
		s_rendererData->CameraUbo->setData(&cameraData);

		// Point Lights
		const std::vector<PointLight>& pointLights = lightsData.PointLights;
		std::vector<GpuPointLightStruct> gpuPointLights;
		gpuPointLights.reserve(pointLights.size());
		for (const auto& pt : pointLights)
			gpuPointLights.push_back(pt);

		// Computing counts / sizes
		s_rendererData->PointLightsCount = pointLights.size();
		uint32_t pointLightsArraySize = 10 * sizeof(GpuPointLightStruct);
		uint32_t bufferSize = pointLightsArraySize + sizeof(s_rendererData->PointLightsCount);
		// Copying point lights data
		uint8_t* lightsDataBuffer = new uint8_t[bufferSize];
		memcpy(lightsDataBuffer, gpuPointLights.data(), pointLightsArraySize);
		memcpy(lightsDataBuffer + pointLightsArraySize, &s_rendererData->PointLightsCount, sizeof(s_rendererData->PointLightsCount));
		s_rendererData->PointLightsUbo->setData(lightsDataBuffer);
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
		std::vector<PointLightInstance> pointLightInstances;
		pointLightInstances.reserve(s_rendererData->PointLightsCount);
		for (int i = 0; i < s_rendererData->PointLightsCount; i++)
			pointLightInstances.emplace_back(i, 0);

		Ref<VertexBuffer> lightInstancesVbo = VertexBuffer::create(pointLightInstances.size() * sizeof(int), pointLightInstances.data());
		Layout lightInstancesLayout = {
			{ 1, "a_lightInstanceIndex", BufferElementType::Int, 1 },
			{ 2, "a_lightInstanceEntityID", BufferElementType::Int, 1 }
		};
		lightInstancesVbo->setBufferLayout(lightInstancesLayout);

		// Light mesh
		Ref<VertexArray> lightVertexArray = VertexArray::create();
		lightVertexArray->addVertexBuffer(s_rendererData->LightMesh->getGeometryVertexBuffer());
		lightVertexArray->addVertexBuffer(lightInstancesVbo);
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