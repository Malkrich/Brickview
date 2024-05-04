#include "Pch.h"
#include "RenderQueue.h"

#include "Core/Core.h"

#include "RenderCommand.h"

namespace Brickview
{
	RenderQueue* RenderQueue::s_instance = nullptr;

	RenderQueue::RenderQueue()
	{
		BV_ASSERT(!s_instance, "s_instance of RenderQueue has already been created !");
	}

	void RenderQueue::init()
	{
		s_instance = new RenderQueue();
	}

	void RenderQueue::shutdown()
	{
		delete s_instance;
		s_instance = nullptr;
	}

	void RenderQueue::submitImpl(
		const std::shared_ptr<VertexArray>& vao,
		const std::shared_ptr<Shader>& shader,
		const UniformMap& uniforms)
	{
		RendererSubmission submission;

		// Create VAO
		submission.Vao = vao;
		submission.Shader = shader;
		submission.Uniforms = uniforms;

		m_renderQueue.push_back(submission);
	}

	void RenderQueue::flushImpl()
	{
		BV_LOG_INFO("Render queue has {} submission.", m_renderQueue.size());

		for (const auto& element : m_renderQueue)
		{
			element.Shader->bind();
			element.Shader->setUniforms(element.Uniforms);

			element.Vao->bind();

			RenderCommand::draw(element.Vao);
		}

		m_renderQueue.clear();
	}

}