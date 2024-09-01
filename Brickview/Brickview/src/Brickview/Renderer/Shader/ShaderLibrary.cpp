#include "Pch.h"
#include "ShaderLibrary.h"

namespace Brickview
{

	const Ref<Shader>& ShaderLibrary::get(const std::string& name) const
	{
		BV_ASSERT(m_shaderRegistry.find(name) != m_shaderRegistry.end(), "Shader not found!");
		const ShaderData& sData = m_shaderRegistry.at(name);
		return sData.Shader;
	}

	void ShaderLibrary::add(const ShaderData& shaderData)
	{
		const std::string& name = shaderData.Shader->getName();
		BV_ASSERT(m_shaderRegistry.find(name) == m_shaderRegistry.end(), "Shader already registered!");

		m_shaderRegistry[name] = shaderData;
	}

	void ShaderLibrary::load(const std::filesystem::path& filePath)
	{
		ShaderData shaderData;
		shaderData.Shader = createRef<Shader>(filePath);
		shaderData.FilePath = filePath;
		add(shaderData);
	}

	void ShaderLibrary::reload(const std::string& shaderName)
	{
		BV_ASSERT(m_shaderRegistry.find(shaderName) != m_shaderRegistry.end(), "Shader not found !");

		ShaderData& sData = m_shaderRegistry.at(shaderName);
		sData.Shader = createRef<Shader>(sData.FilePath);
	}

}