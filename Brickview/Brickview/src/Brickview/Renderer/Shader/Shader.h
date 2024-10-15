#pragma once

#include "Renderer/UniformBuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>

namespace Brickview
{

	class Shader
	{
	public:
		static Ref<Shader> create(const std::filesystem::path& filePath);

		virtual ~Shader() = default;

		virtual const std::string& getName() const = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void reload(const std::filesystem::path& filePath) = 0;

		virtual void setBool(const std::string& name, bool data) = 0;
		virtual void setFloat3(const std::string& name, const glm::vec3& data) = 0;
		virtual void setMat4(const std::string& name, const glm::mat4& data) = 0;
		virtual void setUniformBuffer(Ref<UniformBuffer> uniformBuffer) = 0;
	};

}