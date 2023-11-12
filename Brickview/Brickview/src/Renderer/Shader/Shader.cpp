#include "Shader.h"

#include <iostream>
#include <fstream>
#include <GL\glew.h>

namespace Utils
{
    static std::string readFile(const std::string& fileName)
    {
        std::ifstream file;
        file.open(fileName.c_str());

        if (!file)
            std::cout << "File named " << fileName << " not found !" << std::endl;

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
            std::cout << "ERROR: SHADER COMPILATION FAILED\n" << log << std::endl;
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
                std::cout << "ERROR: SHADER LINKING FAILED\n" << log << std::endl;
            }
        }

        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
    }
}