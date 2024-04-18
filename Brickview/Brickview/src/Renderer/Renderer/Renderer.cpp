#include "Pch.h"
#include "Renderer.h"

#include "Renderer/RenderCommand.h"

namespace Brickview
{

	void Renderer::flush()
	{
		for (const auto& element : m_renderQueue)
		{
			RendererComponent component = element.Component;
			
			if (m_shaderMap.find(component) == m_shaderMap.end())
				continue;

			const auto& shader = m_shaderMap.at(component);
			const auto& vao = element.Vao;

			shader->bind();
			shader->setUniforms(element.Uniforms);

			vao->bind();
			RenderCommand::draw(vao);
		}
	}

}