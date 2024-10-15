#include "Pch.h"
#include "SceneRenderer.h"

#include "RenderCommand.h"
#include "Renderer.h"

namespace Brickview
{
	SceneRenderer::SceneRenderer()
	{
		UniformBufferSpecifications cameraDataUboSpecs;
		cameraDataUboSpecs.BindingPoint = 0;
		cameraDataUboSpecs.Layout = {
			UniformBufferElementType::Mat4,
			UniformBufferElementType::Float3
		};
		m_cameraDataUbo = UniformBuffer::create(cameraDataUboSpecs);
	}

	void SceneRenderer::begin(const Camera& camera)
	{
		m_cameraData.ViewProjectionMatrix = camera.getViewProjectionMatrix();
		m_cameraData.Position = camera.getPosition();
	}

	void SceneRenderer::submitLegoPart(const LegoPartComponent& legoPart, const LegoMeshRegistry& legoPartMeshRegistry, const TransformComponent& transform)
	{
		LegoPartID id = legoPart.ID;
		glm::mat4 transformMat = transform.getTransform();

		if (!m_currentBufferIndex.contains(id))
		{
			insertNewBuffer(id, legoPartMeshRegistry, transformMat);
			return;
		}

		uint32_t instanceBufferIndex = m_currentBufferIndex.at(id);
		InstanceBuffer& buffer = m_instanceBuffers[instanceBufferIndex];

		if (buffer.InstanceCount < buffer.Transforms.size())
		{
			buffer.Transforms[buffer.InstanceCount] = transformMat;
			buffer.InstanceCount++;
		}
		else
		{
			insertNewBuffer(id, legoPartMeshRegistry, transformMat);
		}
	}

	void SceneRenderer::render()
	{
		RenderCommand::setClearColor(0.2f, 0.2f, 0.2f);
		RenderCommand::clear();

		// Camera Uniform buffer
		m_cameraDataUbo->setElement(0, &m_cameraData.ViewProjectionMatrix);
		m_cameraDataUbo->setElement(1, &m_cameraData.Position);
		
		Ref<Shader> solidShader = Renderer::getShaderLibrary()->get("Solid");

		for (const InstanceBuffer& buffer : m_instanceBuffers)
		{
			Renderer::renderMeshInstances(solidShader, buffer.Mesh, buffer.Transforms.data(), buffer.InstanceCount);
		}

		m_currentBufferIndex.clear();
		m_instanceBuffers.clear();
	}


	void SceneRenderer::insertNewBuffer(LegoPartID id, const LegoMeshRegistry& legoMeshRegistry, const glm::mat4& transform)
	{
		// Creates new buffer with geometry
		InstanceBuffer instanceBuffer;
		instanceBuffer.DebugID = id;
		instanceBuffer.Mesh = legoMeshRegistry.getPart(id).Mesh;
		instanceBuffer.Transforms[0] = transform;
		instanceBuffer.InstanceCount++;

		m_instanceBuffers.push_back(instanceBuffer);
		m_currentBufferIndex[id] = m_instanceBuffers.size() - 1;
	}
}