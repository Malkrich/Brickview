#include "Pch.h"
#include "Renderer.h"

#include "Core/Memory/Buffer.h"
#include "Core/Memory/BufferStreamWriter.h"
#include "RendererCore.h"
#include "RenderCommand.h"
#include "FrameBuffer.h"
#include "GpuBuffer.h"
#include "UniformBuffer.h"
#include "ShaderStorageBuffer.h"
#include "TextureUtils.h"

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

	struct ModelDataUboBlock
	{
		glm::mat4 Transform = glm::mat4(1.0f);
		RendererMaterial Material;
		int EntityID = -1;
	};

	struct SkyboxParamUboBlock
	{
		float MipLevel = 0.0f;

		SkyboxParamUboBlock() = default;
	};

	struct RendererData
	{
		// Shaders
		Ref<ShaderLibrary> ShaderLibrary = nullptr;

		// Camera
		Ref<UniformBuffer> CameraUbo = nullptr;

		// Geometry
		Ref<GpuMesh> FullScreenQuad = nullptr;
		Ref<GpuMesh> SkyboxCubeMesh = nullptr;

		// Point Lights
		uint32_t PointLightsCount = 0;
		Ref<ShaderStorageBuffer> LightsDataSsbo = nullptr;
		Ref<VertexBuffer> PointLightInstancesVbo = nullptr;
		std::vector<int> PointLightInstancesData;
		Ref<GpuMesh> LightMesh = nullptr;

		// Environment lighting
		EnvironmentImages EnvironmentImages;
		Ref<TextureCubemap> RenderedSkyboxHandle = nullptr;
		float RenderedSkyboxMipLevel = 0.0f;
		Ref<UniformBuffer> SkyboxParamUbo = nullptr;
		const glm::mat4 CubemapCaptureProjectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		const std::map<CubemapFace, glm::mat4> CubemapCaptureViewMatrices = {
			{ CubemapFace::PositiveX,
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)) },
			{ CubemapFace::NegativeX,
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)) },
			{ CubemapFace::PositiveY,
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)) },
			{ CubemapFace::NegativeY,
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)) },
			{ CubemapFace::PositiveZ,
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)) },
			{ CubemapFace::NegativeZ,
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)) }
		};

		// Meshes
		Ref<UniformBuffer> ModelDataUbo = nullptr;
		Layout MeshVboLayout;
	};

	static RendererData* s_rendererData;

	void Renderer::init()
	{
		s_rendererData = new RendererData();

		// Shaders
		s_rendererData->ShaderLibrary = createRef<ShaderLibrary>();
		std::filesystem::path shaderBaseDir = "data/Shaders/";
		//s_rendererData->ShaderLibrary->load(shaderBaseDir / "SolidMesh.glsl");
		//s_rendererData->ShaderLibrary->load(shaderBaseDir / "PhongLegoMesh.glsl");
		//s_rendererData->ShaderLibrary->load(shaderBaseDir / "PBRLegoMesh.glsl");
		s_rendererData->ShaderLibrary->load(shaderBaseDir / "PBRMesh.glsl");
		s_rendererData->ShaderLibrary->load(shaderBaseDir / "Light.glsl");
		s_rendererData->ShaderLibrary->load(shaderBaseDir / "Line.glsl");
		s_rendererData->ShaderLibrary->load(shaderBaseDir / "MeshWireframe.glsl");
		s_rendererData->ShaderLibrary->load(shaderBaseDir / "EquirectangularToCubemap.glsl");
		s_rendererData->ShaderLibrary->load(shaderBaseDir / "Skybox.glsl");
		s_rendererData->ShaderLibrary->load(shaderBaseDir / "IrradianceMap.glsl");
		s_rendererData->ShaderLibrary->load(shaderBaseDir / "PreFilteredMap.glsl");
		s_rendererData->ShaderLibrary->load(shaderBaseDir / "BrdfLUTMap.glsl");

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
			{ 1, "a_lightInstanceIndex", BufferElementType::Int, 1 }
		};

		s_rendererData->LightMesh = createRef<GpuMesh>(
			lightMeshVertices.size() * sizeof(LightVertex), lightMeshVertices.data(), 
			pointLightMeshLayout,
			lightMeshIndices.size() * sizeof(TriangleFace), lightMeshIndices.data());
		s_rendererData->PointLightInstancesVbo = VertexBuffer::create(sizeof(int));
		s_rendererData->PointLightInstancesVbo->setBufferLayout(pointLightInstancesLayout);

		ShaderStorageBufferSpecifications lightsDataSsboSpecs;
		lightsDataSsboSpecs.BlockName = "LightsData";
		lightsDataSsboSpecs.BindingPoint = 1;
		s_rendererData->LightsDataSsbo = ShaderStorageBuffer::create(lightsDataSsboSpecs, sizeof(GpuPointLightStruct));

		// Meshes
		UniformBufferSpecifications modelDataUboSpecs;
		modelDataUboSpecs.BlockName = "ModelData";
		modelDataUboSpecs.BindingPoint = 2;
		s_rendererData->ModelDataUbo = UniformBuffer::create(modelDataUboSpecs, sizeof(ModelDataUboBlock));
		s_rendererData->MeshVboLayout = {
			{ 0, "a_position", BufferElementType::Float3 },
			{ 1, "a_normal", BufferElementType::Float3 }
		};

		// Full screen quad
		const std::array<glm::vec2, 4> fullScreenQuadVertices = {
			glm::vec2(-1.0f, -1.0f),
			glm::vec2( 1.0f, -1.0f),
			glm::vec2( 1.0f,  1.0f),
			glm::vec2(-1.0f,  1.0f)
		};
		Layout fullScreenQuadLayout = {
			{ 0, "a_position", BufferElementType::Float2 }
		};
		const std::array<int, 6> fullScreenQuadIndices = {
			0, 1, 2,
			2, 3, 0
		};
		s_rendererData->FullScreenQuad = createRef<GpuMesh>(
			fullScreenQuadVertices.size() * sizeof(glm::vec2), fullScreenQuadVertices.data(), fullScreenQuadLayout,
			fullScreenQuadIndices.size() * sizeof(uint32_t), fullScreenQuadIndices.data());

		// Skybox cube mesh
		const std::array<glm::vec3, 8> skyboxCubeMeshVertices = {
			glm::vec3(-0.5f, -0.5f, -0.5f),
			glm::vec3(0.5f, -0.5f, -0.5f),
			glm::vec3(0.5f, -0.5f,  0.5f),
			glm::vec3(-0.5f, -0.5f,  0.5f),
			glm::vec3(-0.5f,  0.5f,  0.5f),
			glm::vec3(-0.5f,  0.5f, -0.5f),
			glm::vec3(0.5f,  0.5f, -0.5f),
			glm::vec3(0.5f,  0.5f,  0.5f)
		};
		const std::array<uint32_t, 6 * 2 * 3> skyboxCubeMeshIndices = {
			1, 0, 2, 2, 0, 3,
			5, 6, 7, 7, 4, 5,
			3, 0, 5, 5, 4, 3,
			4, 7, 2, 2, 3, 4,
			1, 2, 7, 7, 6, 1,
			0, 1, 6, 6, 5, 0
		};
		Layout skyboxCubeMeshLayout = {
			{ 0, "a_position", BufferElementType::Float3 }
		};
		s_rendererData->SkyboxCubeMesh = createRef<GpuMesh>(
			skyboxCubeMeshVertices.size() * sizeof(glm::vec3), skyboxCubeMeshVertices.data(), skyboxCubeMeshLayout,
			skyboxCubeMeshIndices.size() * sizeof(uint32_t), skyboxCubeMeshIndices.data());
		UniformBufferSpecifications skyboxParamUboSpecs;
		skyboxParamUboSpecs.BlockName = "SkyboxParam";
		skyboxParamUboSpecs.BindingPoint = 1;
		s_rendererData->SkyboxParamUbo = UniformBuffer::create(skyboxParamUboSpecs, sizeof(SkyboxParamUboBlock));

		RenderCommand::enableDepthTesting(true);
		RenderCommand::enableFaceCulling(true);
		RenderCommand::setFaceCullingMode(FaceCullingMode::Back);
		RenderCommand::setFaceWindingMode(FaceWindingMode::CounterClockwise);
	}

	void Renderer::shutdown()
	{
		delete s_rendererData;
		s_rendererData = nullptr;
	}

	void Renderer::begin(const CameraData& cameraData, const RendererEnvironment& env)
	{
		BV_ASSERT(env.PointLights.size() == env.PointLightIDs.size(), "Point light object and ID vectors do not have the same size!");

		// Camera
		s_rendererData->CameraUbo->bind();
		s_rendererData->CameraUbo->setData(&cameraData);

		// Environment map
		s_rendererData->EnvironmentImages = env.Images;
		s_rendererData->RenderedSkyboxHandle = env.RenderedSkyboxHandle;
		s_rendererData->RenderedSkyboxMipLevel = env.RenderedSkyboxMipLevel;

		// Point Lights
		s_rendererData->PointLightsCount = env.PointLights.size();
		std::vector<GpuPointLightStruct> gpuPointLights(s_rendererData->PointLightsCount);
		s_rendererData->PointLightInstancesData.resize(s_rendererData->PointLightsCount);
		for (int i = 0; i < s_rendererData->PointLightsCount; i++)
		{
			gpuPointLights[i] = GpuPointLightStruct(env.PointLights[i]);
			s_rendererData->PointLightInstancesData[i] = env.PointLightIDs[i];
		}

		Buffer lightsDataSsboBuffer;
		BufferStreamWriter stream(lightsDataSsboBuffer);
		stream.writePrimitiveType<uint32_t>(s_rendererData->PointLightsCount);
		stream.writeSpan(3, 0);
		stream.writeVector(gpuPointLights);

		if (lightsDataSsboBuffer.Size != s_rendererData->LightsDataSsbo->getSize())
			s_rendererData->LightsDataSsbo->resize(lightsDataSsboBuffer.Size);
		s_rendererData->LightsDataSsbo->setData(lightsDataSsboBuffer.Data);

		lightsDataSsboBuffer.release();
	}

	void Renderer::end()
	{
		renderPointLights();

		if (s_rendererData->RenderedSkyboxHandle)
			renderSkybox(s_rendererData->RenderedSkyboxHandle);
	}

	EnvironmentImages Renderer::createEnvironmentImages(Ref<Texture2D> hdriTexture, const EnvironmentImagesParameters& environmentImagesParameters)
	{
		uint32_t environmentMapDimXY = environmentImagesParameters.EnvironmentMapDimXY;
		uint32_t irradianceMapDimXY = environmentImagesParameters.IrradianceMapDimXY;
		uint32_t preFilteredMapDimXY = environmentImagesParameters.PreFilteredMapDimXY;
		uint32_t preFilteredMapMipMapLevelCount = environmentImagesParameters.PreFilteredMapMipMapLevelCount;
		uint32_t brdfLUTMapDimXY = environmentImagesParameters.BrdfLUTMapDimXY;

		// Initialize all images
		EnvironmentImages environmentImages;
		// Environment map
		{
			TextureCubemapSpecifications cubemapSpecs;
			cubemapSpecs.Format = TextureFormat::RGBFloat16;
			cubemapSpecs.Width = environmentMapDimXY;
			cubemapSpecs.Height = environmentMapDimXY;
			environmentImages.EnvironmentMap = TextureCubemap::create(cubemapSpecs);
		}
		// Irradiance map
		{
			TextureCubemapSpecifications cubemapSpecs;
			cubemapSpecs.Format = TextureFormat::RGBFloat16;
			cubemapSpecs.Width = irradianceMapDimXY;
			cubemapSpecs.Height = irradianceMapDimXY;
			environmentImages.IrradianceMap = TextureCubemap::create(cubemapSpecs);
		}
		// Pre Filtered map
		{
			TextureCubemapSpecifications cubemapSpecs;
			cubemapSpecs.Format = TextureFormat::RGBFloat16;
			cubemapSpecs.Width = preFilteredMapDimXY;
			cubemapSpecs.Height = preFilteredMapDimXY;
			cubemapSpecs.MinFilter = TextureFilter::LinearMipmapLinear;
			cubemapSpecs.MagFilter = TextureFilter::Linear;
			cubemapSpecs.Levels = preFilteredMapMipMapLevelCount;
			environmentImages.PreFilteredEnvMap = TextureCubemap::create(cubemapSpecs);
		}
		// Brdf LUT map
		{
			Texture2DSpecifications textureSpecs;
			textureSpecs.Format = TextureFormat::RGFloat16;
			textureSpecs.WrappingModeU = TextureWrapMode::ClampToEdge;
			textureSpecs.WrappingModeV = TextureWrapMode::ClampToEdge;
			environmentImages.BrdfLUTTexture = Texture2D::create(textureSpecs, brdfLUTMapDimXY);
		}

		// Skybox cube geometry
		Ref<VertexArray> cubeVao = VertexArray::create();
		cubeVao->addVertexBuffer(s_rendererData->SkyboxCubeMesh->getGeometryVertexBuffer());
		cubeVao->setIndexBuffer(s_rendererData->SkyboxCubeMesh->getGeometryIndexBuffer());

		// Full screen quad geoemtry
		Ref<VertexArray> fullScreenQuadVao = VertexArray::create();
		fullScreenQuadVao->addVertexBuffer(s_rendererData->FullScreenQuad->getGeometryVertexBuffer());
		fullScreenQuadVao->setIndexBuffer(s_rendererData->FullScreenQuad->getGeometryIndexBuffer());

		// Environment and irradiance map passes
		FrameBufferSpecifications diffuseCaptureFboSpecs;
		diffuseCaptureFboSpecs.Width = environmentMapDimXY;
		diffuseCaptureFboSpecs.Height = environmentMapDimXY;
		diffuseCaptureFboSpecs.Attachments = { ColorAttachmentFormat::Depth, ColorAttachmentFormat::Cubemap };
		diffuseCaptureFboSpecs.Attachments[1].ExistingAttachment = environmentImages.EnvironmentMap;
		Ref<FrameBuffer> diffuseCaptureFbo = FrameBuffer::create(diffuseCaptureFboSpecs);
		
		// Uniform buffer containing the view matrices to render each face
		UniformBufferSpecifications cubemapDataUboSpecs;
		cubemapDataUboSpecs.BlockName = "CubemapData";
		cubemapDataUboSpecs.BindingPoint = 0;
		Ref<UniformBuffer> cubemapDataUbo = UniformBuffer::create(cubemapDataUboSpecs, sizeof(glm::mat4));

		// Equirectangular to cubemap shader pass
		RenderCommand::setViewportDimension(diffuseCaptureFbo->getWidth(), diffuseCaptureFbo->getHeight());
		diffuseCaptureFbo->bind();
		{
			Ref<Shader> equirectangularToCubemapShader = s_rendererData->ShaderLibrary->get("EquirectangularToCubemap");
			equirectangularToCubemapShader->bind();
			hdriTexture->bind();
			for (const auto& [face, viewMatrix] : s_rendererData->CubemapCaptureViewMatrices)
			{
				glm::mat4 viewProjectionMatrix = s_rendererData->CubemapCaptureProjectionMatrix * viewMatrix;
				cubemapDataUbo->setData(glm::value_ptr(viewProjectionMatrix));
				diffuseCaptureFbo->attachCubemapFace(0, face);

				RenderCommand::clear();
				RenderCommand::drawIndexed(cubeVao);
			}
		}
		cubeVao->unbind();
		diffuseCaptureFbo->unbind();

		// Invalidate diffuse capture FBO
		diffuseCaptureFbo->setColorAttachment(0, environmentImages.IrradianceMap);
		diffuseCaptureFbo->resize(irradianceMapDimXY, irradianceMapDimXY);

		// Irradiance map pass
		RenderCommand::setViewportDimension(irradianceMapDimXY);
		diffuseCaptureFbo->bind();
		{
			Ref<Shader> irradianceMapShader = s_rendererData->ShaderLibrary->get("IrradianceMap");
			irradianceMapShader->bind();
			environmentImages.EnvironmentMap->bind();
			for (const auto& [face, viewMatrix] : s_rendererData->CubemapCaptureViewMatrices)
			{
				glm::mat4 viewProjectionMatrix = s_rendererData->CubemapCaptureProjectionMatrix * viewMatrix;
				
				cubemapDataUbo->setData(glm::value_ptr(viewProjectionMatrix));
				diffuseCaptureFbo->attachCubemapFace(0, face);

				RenderCommand::clear();
				RenderCommand::drawIndexed(cubeVao);
			}
		}
		cubeVao->unbind();
		diffuseCaptureFbo->unbind();

		// Specular IBL pre process
		UniformBufferSpecifications preFilteredMapDataUboSpecs;
		preFilteredMapDataUboSpecs.BlockName = "PreFilteredMapData";
		preFilteredMapDataUboSpecs.BindingPoint = 1;
		Ref<UniformBuffer> specularMapDataUbo = UniformBuffer::create(preFilteredMapDataUboSpecs, sizeof(float));

		FrameBufferSpecifications preFilteredCaptureFboSpecs;
		preFilteredCaptureFboSpecs.Width = preFilteredMapDimXY;
		preFilteredCaptureFboSpecs.Height = preFilteredMapDimXY;
		preFilteredCaptureFboSpecs.Attachments = { ColorAttachmentFormat::Depth, ColorAttachmentFormat::Cubemap };
		preFilteredCaptureFboSpecs.Attachments[1].ExistingAttachment = environmentImages.PreFilteredEnvMap;
		preFilteredCaptureFboSpecs.Attachments[1].Resizable = false;
		Ref<FrameBuffer> specularCaptureFbo = FrameBuffer::create(preFilteredCaptureFboSpecs);

		// Pre filtered environment map pass
		environmentImages.EnvironmentMap->bind();
		{
			Ref<Shader> preFilteredMapShader = s_rendererData->ShaderLibrary->get("PreFilteredMap");
			preFilteredMapShader->bind();
			for (uint32_t mipmapLevel = 0; mipmapLevel < preFilteredMapMipMapLevelCount; mipmapLevel++)
			{
				uint32_t mipmapDimXY = TextureUtils::computeWidthOrHeightFromMipLevel(mipmapLevel, preFilteredMapDimXY);
				specularCaptureFbo->resize(mipmapDimXY, mipmapDimXY);
				RenderCommand::setViewportDimension(mipmapDimXY);

				float roughness = (float)mipmapLevel / (float)(preFilteredMapMipMapLevelCount - 1);
				specularMapDataUbo->setData(&roughness);

				specularCaptureFbo->bind();
				for (const auto& [face, viewMatrix] : s_rendererData->CubemapCaptureViewMatrices)
				{
					glm::mat4 viewProjectionMatrix = s_rendererData->CubemapCaptureProjectionMatrix * viewMatrix;
					cubemapDataUbo->setData(glm::value_ptr(viewProjectionMatrix));
					specularCaptureFbo->attachCubemapFace(0, face, mipmapLevel);

					RenderCommand::clear();
					RenderCommand::drawIndexed(cubeVao);
					cubeVao->unbind();
				}
			}
		}
		specularCaptureFbo->unbind();

		// BRDF LUT texture pass
		FrameBufferSpecifications brdfLUTMapFboSpecs;
		brdfLUTMapFboSpecs.Width = brdfLUTMapDimXY;
		brdfLUTMapFboSpecs.Height = brdfLUTMapDimXY;
		brdfLUTMapFboSpecs.Attachments = { ColorAttachmentFormat::RGFloat16 };
		brdfLUTMapFboSpecs.Attachments[0].ExistingAttachment = environmentImages.BrdfLUTTexture;
		Ref<FrameBuffer> brdfLUTMapFbo = FrameBuffer::create(brdfLUTMapFboSpecs);

		brdfLUTMapFbo->bind();
		{
			Ref<Shader> brdfLUTMapShader = s_rendererData->ShaderLibrary->get("BrdfLUTMap");
			brdfLUTMapShader->bind();
			RenderCommand::setViewportDimension(brdfLUTMapDimXY);

			RenderCommand::clear();
			RenderCommand::drawIndexed(fullScreenQuadVao);
			fullScreenQuadVao->unbind();
		}
		brdfLUTMapFbo->unbind();

		return environmentImages;
	}

	void Renderer::renderSkybox(Ref<TextureCubemap> cubemap)
	{
		RenderCommand::setDepthFunction(DepthFunction::LessOrEqual);
		RenderCommand::enableCubemapSeamless(true);

		Ref<VertexArray> cubeVao = VertexArray::create();
		cubeVao->addVertexBuffer(s_rendererData->SkyboxCubeMesh->getGeometryVertexBuffer());
		cubeVao->setIndexBuffer(s_rendererData->SkyboxCubeMesh->getGeometryIndexBuffer());

		SkyboxParamUboBlock skyboxParam;
		skyboxParam.MipLevel = s_rendererData->RenderedSkyboxMipLevel;
		s_rendererData->SkyboxParamUbo->bind();
		s_rendererData->SkyboxParamUbo->setData(&skyboxParam);

		Ref<Shader> skyboxShader = s_rendererData->ShaderLibrary->get("Skybox");
		cubemap->bind(0);
		skyboxShader->bind();
		
		cubeVao->bind();
		RenderCommand::drawIndexed(cubeVao);

		cubeVao->unbind();

		RenderCommand::setDepthFunction(DepthFunction::Less);
		RenderCommand::enableCubemapSeamless(false);
	}

	const Ref<ShaderLibrary>& Renderer::getShaderLibrary()
	{
		BV_ASSERT(s_rendererData, "Renderer class is not initialized!");
		return s_rendererData->ShaderLibrary;
	}

	void Renderer::renderMesh(Ref<Shader> shader, const RendererMaterial& material, const Ref<GpuMesh>& mesh, const glm::mat4& transform, int entityID)
	{
		// Model Ubo block
		ModelDataUboBlock modelData;
		modelData.Transform = transform;
		modelData.Material = material;
		modelData.EntityID = entityID;
		s_rendererData->ModelDataUbo->bind();
		s_rendererData->ModelDataUbo->setData(&modelData);

		// Irradiance map
		s_rendererData->EnvironmentImages.IrradianceMap->bind(0);
		s_rendererData->EnvironmentImages.PreFilteredEnvMap->bind(1);
		s_rendererData->EnvironmentImages.BrdfLUTTexture->bind(2);

		// Mesh geometry
		Ref<VertexArray> vao = VertexArray::create();
		vao->addVertexBuffer(mesh->getGeometryVertexBuffer());
		vao->setIndexBuffer(mesh->getGeometryIndexBuffer());

		shader->bind();
		RenderCommand::drawIndexed(vao);
		vao->unbind();
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

	void Renderer::renderMeshWireframe(Ref<GpuMesh> mesh, const glm::mat4& transform, float lineWidth)
	{
		Ref<Shader> wireframeShader = s_rendererData->ShaderLibrary->get("MeshWireframe");

		// Model Ubo block
		ModelDataUboBlock modelData;
		modelData.Transform = transform;
		s_rendererData->ModelDataUbo->setData(&modelData);

		Ref<VertexArray> vao = VertexArray::create();
		vao->addVertexBuffer(mesh->getGeometryVertexBuffer());
		vao->setIndexBuffer(mesh->getGeometryIndexBuffer());

		wireframeShader->bind();
		RenderCommand::setLineWidth(lineWidth);
		RenderCommand::setPolygonMode(PolygonMode::Line);
		RenderCommand::drawIndexed(vao);
		RenderCommand::setPolygonMode(PolygonMode::Fill);
		vao->unbind();
	}

	void Renderer::renderPointLights()
	{
		if (s_rendererData->PointLightsCount == 0)
			return;

		s_rendererData->PointLightInstancesVbo->setData(
			s_rendererData->PointLightsCount * sizeof(int), 
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