#pragma once

#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Buffer/Layout.h"

#include "Models/Mesh.h"
#include "Renderer/Light.h"

namespace Brickview
{
	//enum class RendererComponent
	//{
	//	Mesh,
	//	Light
	//};

	struct RendererSubmission
	{
		//RendererComponent Component;
		std::shared_ptr<VertexArray> Vao;
		std::shared_ptr<Shader> Shader;
		UniformMap Uniforms;
	};

	class RenderQueue
	{
	public:
		static void init();
		static void shutdown();

		static void submit(
			const std::shared_ptr<VertexArray>& vao,
			const std::shared_ptr<Shader>& shader,
			const UniformMap& uniforms)
		{
			s_instance->submitImpl(vao, shader, uniforms);
		}

		static void flush()
		{
			s_instance->flushImpl();
		}

	private:
		RenderQueue();

		void submitImpl(
			const std::shared_ptr<VertexArray>& vao,
			const std::shared_ptr<Shader>& shader,
			const UniformMap& uniforms);

		void flushImpl();

	private:
		std::vector<RendererSubmission> m_renderQueue;

		static RenderQueue* s_instance;
	};

}