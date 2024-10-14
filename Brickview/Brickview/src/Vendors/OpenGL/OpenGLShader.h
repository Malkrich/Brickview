#pragma once

#include "Renderer/Shader/Shader.h"

namespace Brickview
{

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::filesystem::path& filePath);

		virtual ~OpenGLShader();

		virtual const std::string& getName() const { return m_name; }

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void reload(const std::filesystem::path& filePath) override { invalidate(filePath); }

		virtual void setUniforms(const UniformMap& uniforms) override;

	private:
		void invalidate(const std::filesystem::path& filePath);

		void setBool(const std::string& name, const void* data);
		void setFloat3(const std::string& name, const void* data);
		void setMat4(const std::string& name, const void* data);

	private:
		std::string m_name;
		uint32_t m_shaderProgramID = 0;
	};
}