#pragma once

#include "GpuMesh.h"
#include "Shader/Shader.h"
#include "RendererMaterial.h"
#include "Texture2D.h"
#include "FrameBuffer.h"
#include "PerspectiveCamera.h"
#include "Primitives.h"
#include "Lights.h"
#include "Scene/Components.h"
#include "Lego/LegoPartMeshRegistry.h"

#include <glm/glm.hpp>

namespace Brickview
{

	enum class RendererType
	{
		Solid, LightedPhong, LightedPBR
	};

	struct SceneRendererSettings
	{
		// Global
		RendererType RendererType = RendererType::LightedPBR;

		// Grid
		float GridBound = 1.0f;
		float GridStep = 0.1f;
		bool GridDepthTestingEnable = true;
		glm::vec3 GridColor = glm::vec3(0.0f, 0.0f, 0.0f);

		SceneRendererSettings() = default;
	};

	struct InstanceElement
	{
		int EntityID = -1;
		RendererMaterial Material;
		glm::mat4 Transform = glm::mat4(1.0f);

		InstanceElement() = default;
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

	class SceneRenderer
	{
	public:
		SceneRenderer(uint32_t viewportWidth, uint32_t viewportHeight);

		// Viewport and entities
		uint32_t getSceneRenderAttachment() const;
		int32_t getEntityIDAt(uint32_t mouseX, uint32_t mouseY) const;
		void resizeViewport(uint32_t width, uint32_t height);

		// Submission
		void begin(const PerspectiveCamera& camera, const std::vector<PointLight> pointLights);
		// Material should be found from the LegoPartComponent material (LegoMaterial -> LegoMaterialRegistry -> RendererMaterial)
		void submitLegoPart(const LegoPartComponent& legoPart, const LegoPartMeshRegistry& legoPartMeshRegistry, const TransformComponent& transform, const MaterialComponent& materialComponent, uint32_t entityID);
		void render();

		// RendererSettings
		RendererType getRendererType() const { return m_rendererSettings.RendererType; }
		void setRendererType(RendererType rendererType) { m_rendererSettings.RendererType = rendererType; }
		// Grid settings
		bool isGridDepthTestEnable() const { return m_rendererSettings.GridDepthTestingEnable; }
		void setGridDepthTesting(bool enable) { m_rendererSettings.GridDepthTestingEnable = enable; }

	private:
		void init(uint32_t viewportWidth, uint32_t viewportHeight);

		void insertNewInstanceBuffer(LegoPartID id, const Ref<GpuMesh>& mesh, const InstanceElement& firstInstanceElement);

		std::vector<Line> generateGrid(float gridBound, float gridStep);

	private:
		// Renderer internal
		SceneRendererSettings m_rendererSettings;

		// Viewport
		Ref<FrameBuffer> m_viewportFrameBuffer;

		// Lego parts
		std::vector<InstanceBuffer> m_instanceBuffers;
		std::unordered_map<LegoPartID, uint32_t> m_currentBufferIndex;
		Layout m_instanceBufferLayout;

		// Lines
		// Origin helper
		std::vector<Line> m_originLines;
		// Grid
		std::vector<glm::vec3> m_originLineColors;
		std::vector<Line> m_gridLines;
	};

}