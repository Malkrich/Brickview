#pragma once

#include "UniformBuffer.h"
#include "Shader/Shader.h"
#include "Texture2D.h"
#include "FrameBuffer.h"
#include "Camera.h"
#include "Scene/Components.h"
#include "Lego/LegoMeshRegistry.h"

#include <glm/glm.hpp>

namespace Brickview
{

	struct InstanceBuffer
	{
		using TransformBuffer = std::array<glm::mat4, 10>;

		LegoPartID DebugID;

		Ref<GpuMesh> Mesh = nullptr;
		TransformBuffer Transforms = TransformBuffer();
		size_t InstanceCount = 0;

		InstanceBuffer() = default;
	};

	class SceneRenderer
	{
	public:
		SceneRenderer(uint32_t viewportWidth, uint32_t viewportHeight);

		uint32_t getSceneRenderAttachment() const;
		int32_t getEntityIDAt(uint32_t mouseX, uint32_t mouseY) const;
		void resizeViewport(uint32_t width, uint32_t height);

		void begin(const Camera& camera);

		void submitLegoPart(const LegoPartComponent& legoPart, const LegoMeshRegistry& legoPartMeshRegistry, const TransformComponent& transform);

		void render();

	private:
		void insertNewBuffer(LegoPartID id, const LegoMeshRegistry& legoMeshRegistry, const glm::mat4& transform);

	private:
		struct CameraData
		{
			glm::mat4 ViewProjectionMatrix;
			glm::vec3 Position;
		};

	private:
		CameraData m_cameraData;
		Ref<UniformBuffer> m_cameraDataUbo;
		Ref<FrameBuffer> m_viewportFrameBuffer;

		// Lego parts
		std::vector<InstanceBuffer> m_instanceBuffers;
		std::unordered_map<LegoPartID, uint32_t> m_currentBufferIndex;
	};

}