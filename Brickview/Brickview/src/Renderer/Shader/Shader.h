#pragma once

#include <glm/gtc/type_ptr.hpp>


namespace Brickview
{
	enum class UniformType
	{
		Bool,
		Int, Int2, Int3, Int4,
		Float, Float2, Float3, Float4,
		Mat2, Mat3, Mat4,
	};

#define UNIFORM_PTR(data) (void*)glm::value_ptr(data)
	struct UniformData
	{
		UniformData() = default;

		UniformData(const glm::vec3& data)
			: Type(UniformType::Float3)
			, Data(UNIFORM_PTR(data))
		{}
		UniformData(const glm::mat4& data)
			: Type(UniformType::Mat4)
			, Data(UNIFORM_PTR(data))
		{}

		UniformType Type;
		void* Data;
	};

	using UniformMap = std::unordered_map<std::string, UniformData>;

	class Shader
	{
	public:
		Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

		void bind() const;

		void setUniforms(const UniformMap& uniforms);

	private:
		void setFloat3(const std::string& name, const void* data);
		void setMat4(const std::string& name, const void* data);

		void compileAndLink(const std::string& vertexShaderContent, const std::string& fragmentShaderContent);

	private:
		unsigned int m_shaderProgramID;
	};
}