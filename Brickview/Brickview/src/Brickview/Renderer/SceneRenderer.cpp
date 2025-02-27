#include "Pch.h"
#include "SceneRenderer.h"

#include "RenderCommand.h"
#include "Renderer.h"

namespace Brickview
{
	SceneRenderer::SceneRenderer(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		init(viewportWidth, viewportHeight);
	}

	void SceneRenderer::init(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		// Rendered frame buffer
		FrameBufferSpecifications viewportFrameBufferSpecs;
		viewportFrameBufferSpecs.Width = viewportWidth;
		viewportFrameBufferSpecs.Height = viewportHeight;
		viewportFrameBufferSpecs.Attachments = { FrameBufferAttachment::RGBA8, FrameBufferAttachment::RedInt, FrameBufferAttachment::Depth };
		m_viewportFrameBuffer = FrameBuffer::create(viewportFrameBufferSpecs);

		// Mesh instance geometry
		m_instanceBufferLayout = {
			{ 2, "a_entityID", BufferElementType::Int, 1 },
			{ 3, "a_albedo", BufferElementType::Float4, 1 },
			{ 4, "a_roughness", BufferElementType::Float, 1 },
			{ 5, "a_metalness", BufferElementType::Float, 1 },
			{ 6, "a_transform", BufferElementType::Mat4, 1 }
		};

		// Lines
		BV_ASSERT(m_originLines.size() == m_originLineColors.size(), "OriginLines and OriginLineColors must be the same length!");
		m_originLines = {
			Line({ 0.0f, 0.0f, 0.0f }, { 0.1f, 0.0f, 0.0f }),
			Line({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.1f, 0.0f }),
			Line({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.1f })
		};
		// Origin
		m_originLineColors = {
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		};

		// Grid
		m_gridLines = generateGrid(m_rendererSettings.GridBound, m_rendererSettings.GridStep);

		// HDRI environment
		Texture2DSpecifications hdriTextureSpecs;
		hdriTextureSpecs.Format = Texture2DFormat::FloatRGB;
		hdriTextureSpecs.WrappingModeU = Texture2DWrappingMode::Clamp;
		hdriTextureSpecs.WrappingModeV = Texture2DWrappingMode::Clamp;
		Ref<Texture2D> hdriTexture = Texture2D::create(hdriTextureSpecs, "./data/HDRI/metro_noord_2k.hdr");
		m_envCubemap = Renderer::createCubemap(hdriTexture);
	}

	uint32_t SceneRenderer::getSceneRenderAttachment() const
	{
		return m_viewportFrameBuffer->getColorAttachment(0);
	}

	int32_t SceneRenderer::getEntityIDAt(uint32_t mouseX, uint32_t mouseY) const
	{
		m_viewportFrameBuffer->bind();
		int32_t data = m_viewportFrameBuffer->readPixel(1, mouseX, mouseY);
		m_viewportFrameBuffer->unbind();
		return data;
	}

	void SceneRenderer::resizeViewport(uint32_t width, uint32_t height)
	{
		uint32_t currentWidth = m_viewportFrameBuffer->getSpecifications().Width;
		uint32_t currentHeight = m_viewportFrameBuffer->getSpecifications().Height;

		if (width != currentWidth || height != currentHeight)
			m_viewportFrameBuffer->resize(width, height);
	}

	void SceneRenderer::begin(const PerspectiveCamera& camera, const SceneLightsData& lightsData)
	{
		// Camera
		CameraData cameraData;
		cameraData.ViewProjectionMatrix = camera.getViewProjectionMatrix();
		cameraData.View = camera.getViewMatrix();
		cameraData.Projection = camera.getProjectionMatrix();
		cameraData.Position = camera.getPosition();

		RendererEnvironment env;
		env.PointLights = lightsData.PointLights;
		env.PointLightIDs = lightsData.PointLightIDs;
		Renderer::begin(cameraData, env);
	}

	static Ref<Shader> getLegoPartShader(RendererType rendererType)
	{
		switch (rendererType)
		{
			case RendererType::Solid:        return Renderer::getShaderLibrary()->get("SolidMesh");
			case RendererType::LightedPhong: return Renderer::getShaderLibrary()->get("PhongLegoMesh");
			case RendererType::LightedPBR:   return Renderer::getShaderLibrary()->get("PBRLegoMesh");
		}

		BV_ASSERT(false, "Unknown render type!");
		return nullptr;
	}

	void SceneRenderer::render()
	{
		m_viewportFrameBuffer->bind();
		RenderCommand::setViewportDimension(m_viewportFrameBuffer->getSpecifications().Width, m_viewportFrameBuffer->getSpecifications().Height);
		RenderCommand::setClearColor(0.2f, 0.2f, 0.2f);
		RenderCommand::clear();
		m_viewportFrameBuffer->clearAttachment(1, -1);

		Renderer::renderSkybox(m_envCubemap);

		// Outline
		if (m_selectedEntity && m_selectedEntity.hasComponent<TransformComponent>() && m_selectedEntity.hasComponent<MeshComponent>())
		{
			const MeshComponent& mesh = m_selectedEntity.getComponent<MeshComponent>();
			const TransformComponent& transform = m_selectedEntity.getComponent<TransformComponent>();

			RenderCommand::setFaceCullingMode(FaceCullingMode::Front);
			Renderer::renderMeshWireframe(mesh.MeshData, transform.getTransform(), m_rendererSettings.OutlineWidth);
			RenderCommand::setFaceCullingMode(FaceCullingMode::Back);
		}

		// Lego parts rendering
		//Ref<Shader> legoPartShader = getLegoPartShader(m_rendererSettings.RendererType);

		//for (const InstanceBuffer& buffer : m_instanceBuffers)
		//{
		//	Renderer::renderMeshInstances(legoPartShader, buffer.Mesh, (const void*)buffer.InstanceElements.data(),
		//		m_instanceBufferLayout, sizeof(InstanceElement), buffer.InstanceCount);
		//}

		Ref<Shader> meshShader = Renderer::getShaderLibrary()->get("PBRMesh");
		for (const MeshSubmissionData& meshSub : m_meshSubmissions)
		{
			Renderer::renderMesh(meshShader, meshSub.Material, meshSub.Mesh, meshSub.Transform, meshSub.EntityID);
		}

		// TODO: maybe should not be handled by Renderer class but the scene renderer
		Renderer::renderPointLights();

		// Origin
		Renderer::renderLines(m_originLines, m_originLineColors, 2.0f);

		// Grid
		Renderer::renderLines(m_gridLines, m_rendererSettings.GridColor, 1.0f);

		m_viewportFrameBuffer->unbind();

		// Lego Part instances
		m_currentBufferIndex.clear();
		m_instanceBuffers.clear();
		// Meshes
		m_meshSubmissions.clear();
	}

	void SceneRenderer::submitLegoPart(const LegoPartComponent& legoPart, const LegoPartMeshRegistry& legoPartMeshRegistry, const TransformComponent & transform, const MaterialComponent& materialComponent, uint32_t entityID)
	{
		LegoPartID id = legoPart.ID;
		glm::mat4 transformMat = transform.getTransform();
		const RendererMaterial& material = materialComponent.Material;

		if (m_currentBufferIndex.contains(id))
		{
			// Case when the instance buffer is already created
			uint32_t instanceBufferIndex = m_currentBufferIndex.at(id);
			InstanceBuffer& buffer = m_instanceBuffers[instanceBufferIndex];

			if (buffer.InstanceCount < buffer.InstanceElements.size())
			{
				InstanceElement instanceElement;
				instanceElement.EntityID = (int)entityID;
				instanceElement.Material.Albedo = material.Albedo;
				instanceElement.Material.Roughness = material.Roughness;
				instanceElement.Material.Metalness = material.Metalness;
				instanceElement.Transform = transformMat;
				buffer.InstanceElements[buffer.InstanceCount] = instanceElement;
				buffer.InstanceCount++;
				return;
			}
		}

		Ref<GpuMesh> legoPartMesh = legoPartMeshRegistry.getPart(id);
		InstanceElement instanceElement;
		instanceElement.EntityID = (int)entityID;
		instanceElement.Transform = transformMat;
		instanceElement.Material.Albedo = material.Albedo;
		instanceElement.Material.Roughness = material.Roughness;
		instanceElement.Material.Metalness = material.Metalness;
		insertNewInstanceBuffer(id, legoPartMesh, instanceElement);
	}

	void SceneRenderer::submitMesh(const MeshComponent& mesh, const TransformComponent& transform, const MaterialComponent& material, uint32_t entityID)
	{
		glm::mat4 transformMatrix = transform.getTransform();
		m_meshSubmissions.emplace_back(mesh.MeshData, transformMatrix, material.Material, entityID);
	}

	void SceneRenderer::insertNewInstanceBuffer(LegoPartID id, const Ref<GpuMesh>& mesh, const InstanceElement& firstInstanceElement)
	{
		// Creates new buffer with geometry
		InstanceBuffer instanceBuffer;
		instanceBuffer.DebugID = id;
		instanceBuffer.Mesh = mesh;
		instanceBuffer.InstanceElements[0] = firstInstanceElement;
		instanceBuffer.InstanceCount++;

		m_instanceBuffers.push_back(instanceBuffer);
		m_currentBufferIndex[id] = m_instanceBuffers.size() - 1;
	}

	std::vector<Line> SceneRenderer::generateGrid(float gridBound, float gridStep)
	{
		uint32_t lineCount = (uint32_t)(gridBound / gridStep) + 1;
		std::vector<Line> gridLines;
		gridLines.resize(4 * lineCount);
		for (uint32_t lineIdx = 0; lineIdx < lineCount; lineIdx++)
		{
			// From -X to X
			float lineAxisPosition = (float)lineIdx * gridStep;
			Line lineXPos = { { -gridBound, 0.0f, lineAxisPosition }, { gridBound, 0.0f, lineAxisPosition } };
			Line lineYPos = { { lineAxisPosition, 0.0f, -gridBound }, { lineAxisPosition, 0.0f, gridBound } };
			Line lineXNeg = { { -gridBound, 0.0f, -lineAxisPosition }, { gridBound, 0.0f, -lineAxisPosition } };
			Line lineYNeg = { { -lineAxisPosition, 0.0f, -gridBound }, { -lineAxisPosition, 0.0f, gridBound } };
			gridLines[4 * lineIdx + 0] = lineXPos;
			gridLines[4 * lineIdx + 1] = lineYPos;
			gridLines[4 * lineIdx + 2] = lineXNeg;
			gridLines[4 * lineIdx + 3] = lineYNeg;
		}

		return gridLines;
	}
}