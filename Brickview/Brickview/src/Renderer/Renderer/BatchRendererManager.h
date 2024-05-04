#pragma once

#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Buffer/Layout.h"

#include "Models/Mesh.h"
#include "Renderer/Light.h"

namespace Brickview
{
	struct BufferGroup
	{
		std::shared_ptr<VertexArray>  Vao = nullptr;
		std::shared_ptr<VertexBuffer> Vbo = nullptr;
		std::shared_ptr<IndexBuffer>  Ebo = nullptr;
	};

	struct RenderSubmission
	{
		BufferGroup Buffers;
		std::shared_ptr<Shader> Shader;
		UniformMap Uniforms;
		bool Visible = true;
	};

	class BatchRendererManager
	{
	public:
		BatchRendererManager() = default;

		void addSubmission(const std::string& name,
			uint32_t vertexBufferSize, uint32_t indexBufferSize,
			const Layout& layout,
			const std::shared_ptr<Shader>& shader);

		void setData(const std::string& name,
			uint32_t vertexBufferSize, void* vertexBufferData,
			uint32_t indexBufferSize, void* indexBufferData);
		void setUniforms(const std::string& name, const UniformMap& uniforms);
		void setVisible(const std::string& name, bool visible);

		void flush();

	private:
		std::map<std::string, RenderSubmission> m_renderSubmissions;
	};

}