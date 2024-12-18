#include "Pch.h"
#include "SceneRenderer.h"

#include "RenderCommand.h"
#include "Renderer.h"

namespace Brickview
{
	SceneRenderer::SceneRenderer(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		UniformBufferSpecifications cameraDataUboSpecs;
		cameraDataUboSpecs.BindingPoint = 0;
		cameraDataUboSpecs.Layout = {
			UniformBufferElementType::Mat4,
			UniformBufferElementType::Float3
		};
		m_cameraDataUbo = UniformBuffer::create(cameraDataUboSpecs);

		FrameBufferSpecifications viewportFrameBufferSpecs;
		viewportFrameBufferSpecs.Width = viewportWidth;
		viewportFrameBufferSpecs.Height = viewportHeight;
		viewportFrameBufferSpecs.Attachments = { FrameBufferAttachment::RGBA8, FrameBufferAttachment::RedInt, FrameBufferAttachment::Depth };
		m_viewportFrameBuffer = FrameBuffer::create(viewportFrameBufferSpecs);

		m_instanceBufferLayout = {
			{ 2, "a_entityID", BufferElementType::Int, 1 },
			{ 3, "a_transform", BufferElementType::Mat4, 1 }
		};

		BV_ASSERT(m_originLines.size() == m_originLineColors.size(), "OriginLines and OriginLineColors must be the same length!");
		m_originLines = {
			Line({ 0.0f, 0.0f, 0.0f }, { 0.1f, 0.0f, 0.0f }),
			Line({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.1f, 0.0f }),
			Line({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.1f })
		};
		m_originLineColors = {
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		};

		m_gridLines = generateGrid(m_gridSettings.Bound, m_gridSettings.Step);
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

	void SceneRenderer::begin(const PerspectiveCamera& camera)
	{
		m_cameraData.ViewProjectionMatrix = camera.getViewProjectionMatrix();
		m_cameraData.Position = camera.getPosition();
	}

	void SceneRenderer::submitLegoPart(const LegoPartComponent& legoPart, const LegoMeshRegistry& legoPartMeshRegistry, const TransformComponent& transform, uint32_t entityID)
	{
		LegoPartID id = legoPart.ID;
		glm::mat4 transformMat = transform.getTransform();

		if (!m_currentBufferIndex.contains(id))
		{
			insertNewBuffer(id, legoPartMeshRegistry, InstanceElement(transformMat, (int)entityID));
			return;
		}

		uint32_t instanceBufferIndex = m_currentBufferIndex.at(id);
		InstanceBuffer& buffer = m_instanceBuffers[instanceBufferIndex];

		if (buffer.InstanceCount < buffer.InstanceElements.size())
		{
			buffer.InstanceElements[buffer.InstanceCount] = InstanceElement(transformMat, (int)entityID);
			buffer.InstanceCount++;
		}
		else
		{
			insertNewBuffer(id, legoPartMeshRegistry, InstanceElement(transformMat, (int)entityID));
		}
	}

	void SceneRenderer::render()
	{
		m_viewportFrameBuffer->bind();
		RenderCommand::setClearColor(0.2f, 0.2f, 0.2f);
		RenderCommand::clear();
		m_viewportFrameBuffer->clearAttachment(1, -1);

		// Camera Uniform buffer
		m_cameraDataUbo->setElement(0, &m_cameraData.ViewProjectionMatrix);
		m_cameraDataUbo->setElement(1, &m_cameraData.Position);
		
		Ref<Shader> solidShader = Renderer::getShaderLibrary()->get("SolidMesh");

		// TEMP: move this to render pass
		RenderCommand::enableDepthTesting(true);

		// Lego parts
		for (const InstanceBuffer& buffer : m_instanceBuffers)
		{
			Renderer::renderMeshInstances(solidShader, buffer.Mesh, (const void*)buffer.InstanceElements.data(), 
				m_instanceBufferLayout, sizeof(InstanceElement), buffer.InstanceCount);
		}

		// Origin
		Renderer::renderLines(m_originLines, m_originLineColors, 2.0f);

		// Grid
		RenderCommand::enableDepthTesting(m_gridSettings.DepthTestingEnable);
		Renderer::renderLines(m_gridLines, m_gridSettings.Color, 1.0f);

		m_viewportFrameBuffer->unbind();

		m_currentBufferIndex.clear();
		m_instanceBuffers.clear();
	}


	void SceneRenderer::insertNewBuffer(LegoPartID id, const LegoMeshRegistry& legoMeshRegistry, const InstanceElement& instanceElement)
	{
		// Creates new buffer with geometry
		InstanceBuffer instanceBuffer;
		instanceBuffer.DebugID = id;
		instanceBuffer.Mesh = legoMeshRegistry.getPart(id).Mesh;
		instanceBuffer.InstanceElements[0] = instanceElement;
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