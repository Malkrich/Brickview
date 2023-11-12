#pragma once

#include <string>

namespace Brickview
{
	class Shader
	{
	public:
		Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

		void bind() const;

	private:
		void compileAndLink(const std::string& vertexShaderContent, const std::string& fragmentShaderContent);

	private:
		unsigned int m_shaderProgramID;
	};
}