#pragma once

#include "Renderer/Shader/Shader.h"

#include <filesystem>

namespace Brickview
{

	class ShaderLibrary
	{
	public:
		const Ref<Shader>& get(const std::string& name) const;

		void add(const Ref<Shader>& shader);

		void load(const std::filesystem::path& filePath);

	private:
		std::unordered_map<std::string, Ref<Shader>> m_shaderRegistry;
	};

}