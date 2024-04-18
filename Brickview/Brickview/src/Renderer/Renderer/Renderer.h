#pragma once

#include "Renderer/Shader/Shader.h"
#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Buffer/Layout.h"

#include "Models/Mesh.h"
#include "Renderer/Light.h"

namespace Brickview
{
	enum class RendererComponent
	{
		Mesh,
		Light
	};

	struct RendererSubmission
	{
		RendererComponent Component;
		std::shared_ptr<VertexArray> Vao;
		UniformMap Uniforms;
	};

	class Renderer
	{
	public:
		virtual void submitMesh(const Mesh& mesh) = 0;
		virtual void submitLight(const Light& light) = 0;

		void addShader(RendererComponent componenent, std::shared_ptr<Shader> shader) { m_shaderMap[componenent] = shader; }
		void addToQueue(const RendererSubmission& submission) { m_renderQueue.push_back(submission); }

		void flush();
	public:
		std::unordered_map<RendererComponent, std::shared_ptr<Shader>> m_shaderMap;
		std::vector<RendererSubmission> m_renderQueue;
	};

}