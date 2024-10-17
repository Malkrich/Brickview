#pragma once

#include "Renderer/Shader/ShaderLibrary.h"
#include "GpuMesh.h"
#include "Shader/Shader.h"

#include <glm/glm.hpp>

namespace Brickview
{

	class Renderer
	{
	public:
		static void init();
		static void shutdown();

		// Read only
		static const Ref<ShaderLibrary>& getShaderLibrary();

		static void renderMeshInstances(Ref<Shader> shader, const Ref<GpuMesh>& mesh,
			const void* instanceBufferData, const Layout& instanceBufferLayout, uint32_t instanceBufferSize, size_t instanceCount);
	};

}