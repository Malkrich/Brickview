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
		static Ref<Shader> create(const std::filesystem::path& filePath);

		virtual ~Shader() = default;

		virtual const std::string& getName() const = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void reload(const std::filesystem::path& filePath) = 0;

		virtual void setUniforms(const UniformMap& uniforms) = 0;
	};

}