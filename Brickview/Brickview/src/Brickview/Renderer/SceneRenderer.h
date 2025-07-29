#pragma once

#include "Renderer.h"
#include "GpuMesh.h"
#include "Shader/Shader.h"
#include "RendererMaterial.h"
#include "FrameBuffer.h"
#include "ShaderStorageBuffer.h"
#include "PerspectiveCamera.h"
#include "Texture2D.h"
#include "Primitives.h"
#include "Lights.h"
#include "TextureCubemap.h"
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

	struct DirectionalLightData
	{
		bool Enable = false;
		glm::vec3 Direction = { 0.0f, 0.0f, -1.0f };
	};

	struct SceneLightsData
	{
		std::vector<PointLight> PointLights;
		std::vector<int> PointLightIDs;

		// Not implemented yet
		DirectionalLightData DirectionalLight;
	};

	enum class SkyboxType
	{
		// TODO: ClearColor = 0,
		EnvironmentMap,
		IrradianceMap,
		PreFilteredMap
	};

	class SceneRenderer
	{
	public:
		struct Settings
		{
			// Global
			RendererType RendererType = RendererType::LightedPBR;

			// Grid
			float GridBound = 1.0f;
			float GridStep = 0.1f;
			glm::vec3 GridColor = glm::vec3(0.0f, 0.0f, 0.0f);

			// Wireframe
			float OutlineWidth = 1.0f;

			// Skybox
			SkyboxType Skybox = SkyboxType::IrradianceMap;
			float SkyboxMipLevel = 0.0f;

			Settings() = default;
		};

	public:
		SceneRenderer(uint32_t viewportWidth, uint32_t viewportHeight);

		// Viewport and entities
		uint32_t getSceneRenderAttachment() const;
		int32_t getEntityIDAt(uint32_t mouseX, uint32_t mouseY) const;
		void resizeViewport(uint32_t width, uint32_t height);

		void render();

		void setHdriTexture(Ref<Texture2D> hdriTexture);

		// Submission
		// Environment
		void setSceneEnvironment(const PerspectiveCamera& camera, const SceneLightsData& env);
		// Material should be found from the LegoPartComponent material (LegoMaterial -> LegoMaterialRegistry -> RendererMaterial)
		// Lego parts
		void submitLegoPart(const LegoPartComponent& legoPart, const LegoPartMeshRegistry& legoPartMeshRegistry, const TransformComponent& transform, const MaterialComponent& materialComponent, uint32_t entityID);
		// Meshes
		void submitMesh(const MeshComponent& mesh, const TransformComponent& transform, const MaterialComponent& material, uint32_t entityID);

		Settings& getRendererSettings() { return m_rendererSettings; }

		void setSelectedEntity(Entity e) { m_selectedEntity = e; }

	private:
		void init(uint32_t viewportWidth, uint32_t viewportHeight);

		void insertNewInstanceBuffer(LegoPartID id, const Ref<GpuMesh>& mesh, const InstanceElement& firstInstanceElement);

		std::vector<Line> generateGrid(float gridBound, float gridStep);

		void computeHdriEnvironmentPass(Ref<Texture2D> hdriTexture);

		Ref<TextureCubemap> getSkyboxToRender();

	private:
		// Renderer internal
		Settings m_rendererSettings;

		// Selected entity
		Entity m_selectedEntity;

		// Render target
		Ref<FrameBuffer> m_viewportFrameBuffer = nullptr;

		// Environmnent
		CameraData m_cameraData;
		RendererEnvironment m_environment;

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