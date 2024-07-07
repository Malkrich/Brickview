#pragma once

#include "Core/BasicTypes.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>

namespace Brickview
{
	struct UniformData
	{
		BasicTypes Type;
		void* Data;

		UniformData()
			: Type(BasicTypes::None)
			, Data(nullptr)
		{}
		UniformData(const bool& data)
			: Type(BasicTypes::Bool)
			, Data((void*)(&data))
		{}
		UniformData(const glm::vec3& data)
			: Type(BasicTypes::Float3)
			, Data((void*)glm::value_ptr(data))
		{}
		UniformData(const glm::mat4& data)
			: Type(BasicTypes::Mat4)
			, Data((void*)glm::value_ptr(data))
		{}
	};

	using UniformMap = std::unordered_map<std::string, UniformData>;

	class Shader
	{
	public:
		Shader(const std::filesystem::path& filePath);
		~Shader();

		const std::string& getName() const { return m_name; }

		void bind() const;
		void unbind() const;

		void reload(const std::filesystem::path& filePath) { invalidate(filePath); }

		void setUniforms(const UniformMap& uniforms);

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