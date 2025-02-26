#pragma once

#include "GpuMesh.h"
#include "Shader/Shader.h"
#include "RendererMaterial.h"
#include "FrameBuffer.h"
#include "ShaderStorageBuffer.h"
#include "PerspectiveCamera.h"
#include "Primitives.h"
#include "Lights.h"
#include "Lego/LegoPartMeshRegistry.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

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
		glm::vec3 GridColor = glm::vec3(0.0f, 0.0f, 0.0f);

		// Wireframe
		float OutlineWidth = 3.5f;

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

	struct MeshSubmissionData
	{
		Ref<GpuMesh> Mesh = nullptr;
		glm::mat4 Transform = glm::mat4(1.0f);
		RendererMaterial Material;
		int EntityID = -1;

		MeshSubmissionData() = default;
		MeshSubmissionData(Ref<GpuMesh> mesh, const glm::mat4& transform, const RendererMaterial& material, int entityID)
			: Mesh(mesh)
			, Transform(transform)
			, Material(material) 
			, EntityID(entityID){}
	};

	struct SceneEnvironment
	{
		std::vector<PointLight> PointLights;
		std::vector<int> PointLightIDs;
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
		void begin(const PerspectiveCamera& camera, const SceneEnvironment& env);
		void render();

		// Material should be found from the LegoPartComponent material (LegoMaterial -> LegoMaterialRegistry -> RendererMaterial)
		void submitLegoPart(const LegoPartComponent& legoPart, const LegoPartMeshRegistry& legoPartMeshRegistry, const TransformComponent& transform, const MaterialComponent& materialComponent, uint32_t entityID);
		void submitMesh(const MeshComponent& mesh, const TransformComponent& transform, const MaterialComponent& material, uint32_t entityID);

		SceneRendererSettings& getRendererSettings() { return m_rendererSettings; }

		void setSelectedEntity(Entity e) { m_selectedEntity = e; }

	private:
		void init(uint32_t viewportWidth, uint32_t viewportHeight);

		void insertNewInstanceBuffer(LegoPartID id, const Ref<GpuMesh>& mesh, const InstanceElement& firstInstanceElement);

		std::vector<Line> generateGrid(float gridBound, float gridStep);

	private:
		// Renderer internal
		SceneRendererSettings m_rendererSettings;

		// Selected entity
		Entity m_selectedEntity;

		// Viewport
		Ref<FrameBuffer> m_viewportFrameBuffer;

		// Lego parts
		std::vector<InstanceBuffer> m_instanceBuffers;
		std::unordered_map<LegoPartID, uint32_t> m_currentBufferIndex;
		Layout m_instanceBufferLayout;

		// Meshes
		std::vector<MeshSubmissionData> m_meshSubmissions;

		// Lines
		// Origin helper
		std::vector<Line> m_originLines;
		// Grid
		std::vector<glm::vec3> m_originLineColors;
		std::vector<Line> m_gridLines;
	};

}