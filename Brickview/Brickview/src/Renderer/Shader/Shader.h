#pragma once

#include <glm/gtc/type_ptr.hpp>

namespace Brickview
{
	enum class UniformType
	{
		None = 0,
		Bool,
		Int, Int2, Int3, Int4,
		Float, Float2, Float3, Float4,
		Mat2, Mat3, Mat4,
	};

	struct UniformData
	{
		UniformData() = default;
		UniformData::UniformData(const glm::vec3& data);
		UniformData::UniformData(const glm::mat4& data);

		UniformType Type = UniformType::None;
		void* Data = nullptr;
	};

	using UniformMap = std::unordered_map<std::string, UniformData>;

	class Shader
	{
	public:
		Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

		void bind() const;
		void unbind() const;

		void setUniforms(const UniformMap& uniforms);

	private:
		void setFloat3(const std::string& name, const void* data);
		void setMat4(const std::string& name, const void* data);

		void compileAndLink(const std::string& vertexShaderContent, const std::string& fragmentShaderContent);

	private:
		uint32_t m_shaderProgramID;
	};
}