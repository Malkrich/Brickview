#pragma once

#include "RenderSystem.h"
#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Shader/ShaderLibrary.h"
#include "Renderer/Camera.h"
#include "Renderer/Mesh.h"
#include "Renderer/GpuMesh.h"
#include "Renderer/Material.h"
#include "Lego/LegoPart.h"

#include <glm/glm.hpp>

namespace Brickview
{

	using TransformBuffer = std::array<glm::mat4, 1000>;

	struct InstanceData
	{
		Ref<GpuMesh> Mesh = nullptr;
		TransformBuffer InstanceTransforms;
		size_t InstanceCount = 0;
	};

	class SolidRenderSystem : public RenderSystem
	{
	public:
		SolidRenderSystem(const Ref<ShaderLibrary>& shaderLib);
		virtual ~SolidRenderSystem();

		virtual void begin(const Camera& camera, const Light& light) override;
		virtual void end() override;

		virtual void drawLegoPart(const LegoPartComponent& legoPart, const glm::mat4& transform) override;

	private:
		void flush(const InstanceData& instanceData);

	private:
		Ref<Shader> m_solidShader;

		// Camera data
		UniformMap m_uniforms;
		glm::mat4 m_viewProjectionMatrix;
		glm::vec3 m_cameraPosition;

		// Instance manager
		std::unordered_map<LegoPartID, InstanceData> m_instanceRegistry;
	};

}