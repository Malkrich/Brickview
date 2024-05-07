#include "Pch.h"
#include "ShaderLibrary.h"

namespace Brickview
{

	const Ref<Shader>& ShaderLibrary::get(const std::string& name) const
	{
		BV_ASSERT(m_shaderRegistry.find(name) != m_shaderRegistry.end(), "Shader not found!");
		return m_shaderRegistry.at(name);
	}

	void ShaderLibrary::add(const Ref<Shader>& shader)
	{
		const std::string& name = shader->getName();
		BV_ASSERT(m_shaderRegistry.find(name) == m_shaderRegistry.end(), "Shader already registered!");

		m_shaderRegistry[name] = shader;
	}

	void ShaderLibrary::load(const std::filesystem::path& filePath)
	{
		Ref<Shader> shader = createRef<Shader>(filePath);
		add(shader);
	}

}