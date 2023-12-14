#pragma once

namespace Brickview
{
	class Shader
	{
	public:
		Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

		void bind() const;

		void setUniformVec3(const std::string& name, const float* data);

	private:
		void compileAndLink(const std::string& vertexShaderContent, const std::string& fragmentShaderContent);

	private:
		unsigned int m_shaderProgramID;
	};
}