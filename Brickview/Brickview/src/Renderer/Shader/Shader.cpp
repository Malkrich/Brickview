#include "Pch.h"
#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Log.h"


namespace Utils
{
    static std::string readFile(const std::string& fileName)
    {
        std::ifstream file;
        file.open(fileName.c_str());

        if (!file)
            BV_LOG_ERROR("File named {0} not found !", fileName);

        return std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    }
}

namespace Brickview
{
    static unsigned int compileShader(const std::string& shaderContent, GLenum shaderType)
    {
        int success;
        char log[128];

        int shaderID;
        shaderID = glCreateShader(shaderType);

        const char* rawShaderContent = shaderContent.c_str();
        glShaderSource(shaderID, 1, &rawShaderContent, NULL);

        glCompileShader(shaderID);

        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shaderID, 128, nullptr, log);
            BV_LOG_WARN("Shader compilation failed !");
        }

        return shaderID;
    }

	Shader::Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
	{
        std::string vertexShaderContent = Utils::readFile(vertexShaderFilePath);
        std::string fragmentShaderContent = Utils::readFile(fragmentShaderFilePath);
        compileAndLink(vertexShaderContent, fragmentShaderContent);
	}

    void Shader::bind() const
    {
        glUseProgram(m_shaderProgramID);
    }

    void Shader::setUniformVec3(const std::string& name, const float* data)
    {
        // set uniform color
        int loc = glGetUniformLocation(m_shaderProgramID, name.c_str());
        glUniform3fv(loc, 1, data);
    }

    void Shader::setMat4(const std::string& name, const glm::mat4& data)
    {
        int loc = glGetUniformLocation(m_shaderProgramID, name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(data));
    }

    void Shader::compileAndLink(const std::string& vertexShaderContent, const std::string& fragmetShaderContent)
    {
        unsigned int vertexShaderID = compileShader(vertexShaderContent, GL_VERTEX_SHADER);
        unsigned int fragmentShaderID = compileShader(fragmetShaderContent, GL_FRAGMENT_SHADER);

        m_shaderProgramID = glCreateProgram();
        glAttachShader(m_shaderProgramID, vertexShaderID);
        glAttachShader(m_shaderProgramID, fragmentShaderID);

        glLinkProgram(m_shaderProgramID);

        // DEBUG
        {
            int success;
            char log[128];
            glGetProgramiv(m_shaderProgramID, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(m_shaderProgramID, 128, NULL, log);
                BV_LOG_WARN("Shader program linking failed !");
            }
        }

        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
    }
}