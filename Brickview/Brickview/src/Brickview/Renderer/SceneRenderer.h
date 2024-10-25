#pragma once

#include "Buffer.h"
#include "UniformBuffer.h"
#include "Shader/Shader.h"
#include "Texture2D.h"
#include "FrameBuffer.h"
#include "PerspectiveCamera.h"
#include "Primitives.h"
#include "Scene/Components.h"
#include "Lego/LegoMeshRegistry.h"

#include <glm/glm.hpp>

namespace Brickview
{
	struct EditorGridSettings
	{
		float Bound = 1.0f;
		float Step = 0.1f;
		bool DepthTestingEnable = true;
		glm::vec3 Color = glm::vec3(0.0f, 0.0f, 0.0f);

		EditorGridSettings() = default;
	};

	struct InstanceElement
	{
		int EntityID = -1;
		glm::mat4 Transform = glm::mat4(1.0f);

		InstanceElement() = default;
		InstanceElement(const InstanceElement&) = default;
		InstanceElement(const glm::mat4& transform, int entityID)
			: Transform(transform), EntityID(entityID) {}
	};

	struct InstanceBuffer
	{
		// TODO: switch to uint32_t type
		LegoPartID DebugID = "";

		Ref<GpuMesh> Mesh = nullptr;
		std::array<InstanceElement, 10> InstanceElements;
		size_t InstanceCount = 0;

		InstanceBuffer() = default;
	};

	struct Line;

	class SceneRenderer
	{
	public:
		SceneRenderer(uint32_t viewportWidth, uint32_t viewportHeight);

		// Viewport and entities
		uint32_t getSceneRenderAttachment() const;
		int32_t getEntityIDAt(uint32_t mouseX, uint32_t mouseY) const;
		void resizeViewport(uint32_t width, uint32_t height);

		// Submission
		void begin(const PerspectiveCamera& camera);
		void submitLegoPart(const LegoPartComponent& legoPart, const LegoMeshRegistry& legoPartMeshRegistry, const TransformComponent& transform, uint32_t entityID);
		void render();

		// Grid
		bool isGridDepthTestEnable() const { return m_gridSettings.DepthTestingEnable; }
		void setGridDepthTesting(bool enable) { m_gridSettings.DepthTestingEnable = enable; }

	private:
		void insertNewBuffer(LegoPartID id, const LegoMeshRegistry& legoMeshRegistry, const InstanceElement& instanceElement);

		std::vector<Line> generateGrid(float gridBound, float gridStep);

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
		Layout m_instanceBufferLayout;

		// Origin helper
		std::vector<Line> m_originLines;
		std::vector<glm::vec3> m_originLineColors;
		// Grid
		EditorGridSettings m_gridSettings;
		std::vector<Line> m_gridLines;
	};

}