#include "Pch.h"
#include "BatchRendererManager.h"

#include "Core/Core.h"

#include "RenderCommand.h"

namespace Brickview
{

	void BatchRendererManager::addSubmission(const std::string& name,
		uint32_t vertexBufferSize, uint32_t indexBufferSize,
		const Layout& layout,
		const std::shared_ptr<Shader>& shader)
	{
		RenderSubmission submission;
		
		// Buffers
		BufferGroup buffers;
		buffers.Vbo = std::make_shared<VertexBuffer>(vertexBufferSize);
		buffers.Vbo->setBufferLayout(layout);
		buffers.Ebo = std::make_shared<IndexBuffer>(indexBufferSize);

		buffers.Vao = std::make_shared<VertexArray>();
		buffers.Vao->addVertexBuffer(buffers.Vbo);
		buffers.Vao->setIndexBuffer(buffers.Ebo);
		buffers.Vao->unbind();
		submission.Buffers = buffers;

		// Shader
		submission.Shader = shader;
		
		m_renderSubmissions[name] = submission;
	}

	void BatchRendererManager::setData(const std::string& name, uint32_t vertexBufferSize, void* vertexBufferData, uint32_t indexBufferSize, void* indexBufferData)
	{
		BV_ASSERT(m_renderSubmissions.find(name) != m_renderSubmissions.end(), "Submission name doesn't exist !");

		auto& submission = m_renderSubmissions.at(name);
		submission.Buffers.Vao->bind();
		submission.Buffers.Vbo->setData(vertexBufferSize, vertexBufferData);
		submission.Buffers.Ebo->setData(indexBufferSize, indexBufferData);
		submission.Buffers.Vao->unbind();
	}

	void BatchRendererManager::setUniforms(const std::string& name, const UniformMap& uniforms)
	{
		BV_ASSERT(m_renderSubmissions.find(name) != m_renderSubmissions.end(), "Submission name doesn't exist !");

		auto& submission = m_renderSubmissions.at(name);
		submission.Uniforms = uniforms;
	}

	void BatchRendererManager::setVisible(const std::string& name, bool visible)
	{
		BV_ASSERT(m_renderSubmissions.find(name) != m_renderSubmissions.end(), "Submission name doesn't exist !");

		auto& submission = m_renderSubmissions.at(name);
		submission.Visible = visible;
	}

	void BatchRendererManager::flush()
	{
		for (const auto& [name, submission] : m_renderSubmissions)
		{
			if (!submission.Visible)
				continue;

			submission.Shader->bind();
			submission.Shader->setUniforms(submission.Uniforms);

			RenderCommand::draw(submission.Buffers.Vao);

			submission.Buffers.Vao->unbind();
		}
	}

}