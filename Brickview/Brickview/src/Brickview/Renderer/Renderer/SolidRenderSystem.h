#pragma once

#include "RenderSystem.h"
#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Shader/ShaderLibrary.h"
#include "Renderer/Camera.h"
#include "Renderer/GpuMesh.h"
#include "Renderer/Material.h"
#include "Lego/LegoPart.h"

#include <glm/glm.hpp>

namespace Brickview
{
	
	struct InstanceData;

	class SolidRenderSystem : public RenderSystem
	{
	public:
		SolidRenderSystem(const Ref<ShaderLibrary>& shaderLib);
		virtual ~SolidRenderSystem();

		virtual void begin(const Camera& camera, const Light& light) override;
		virtual void end() override;

		virtual const RenderStatistics& getRenderStatistics() const { return m_renderStatistics; }

		virtual void drawLegoPart(const LegoPartComponent& legoPart, const glm::mat4& transform) override;

	private:
		void flush(const InstanceData& instanceData);

		void resetStats();

	private:
		Ref<Shader> m_solidShader;

		// Camera data
		UniformMap m_uniforms;
		glm::mat4 m_viewProjectionMatrix;
		glm::vec3 m_cameraPosition;

		// Instance manager
		std::unordered_map<LegoPartID, InstanceData> m_instanceRegistry;

		// stats
		RenderStatistics m_renderStatistics;
	};

}