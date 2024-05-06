#pragma once

#include "Renderer/Shader/Shader.h"

#include <filesystem>

namespace Brickview
{

	class ShaderLibrary
	{
	public:
		//ShaderLibrary();

		//const Ref<Shader>& getShader(const std::string& name) const;

		//void addShader(const std::filesystem::path& filePath);

	private:
		std::unordered_map<std::string, Ref<Shader>> m_shaderRegistry;
	};

}