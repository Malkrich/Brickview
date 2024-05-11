#pragma once

#include "Renderer/Shader/Shader.h"

#include <filesystem>

namespace Brickview
{

	struct ShaderData
	{
		std::filesystem::path FilePath;
		Ref<Shader> Shader;
	};

	class ShaderLibrary
	{
	public:
		const Ref<Shader>& get(const std::string& name) const;

		void add(const ShaderData& shader);

		void load(const std::filesystem::path& filePath);
		void reload(const std::string& shaderName);

		std::unordered_map<std::string, ShaderData>::const_iterator begin() const { return m_shaderRegistry.begin(); }
		std::unordered_map<std::string, ShaderData>::const_iterator end() const { return m_shaderRegistry.end(); }
		std::unordered_map<std::string, ShaderData>::iterator begin() { return m_shaderRegistry.begin(); }
		std::unordered_map<std::string, ShaderData>::iterator end() { return m_shaderRegistry.end(); }

	private:
		std::unordered_map<std::string, ShaderData> m_shaderRegistry;

	};

}