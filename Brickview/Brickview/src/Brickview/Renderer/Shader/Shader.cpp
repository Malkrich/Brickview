#include "Pch.h"
#include "Renderer/OpenGLError.h"
#include "Shader.h"

#include <glad/glad.h>

namespace Brickview
{
    namespace Utils
    {

        static std::string readFile(const std::filesystem::path& filePath)
        {
            std::ifstream file;
            auto filePathStr = filePath.generic_string();
            file.open(filePathStr.c_str());

            if (!file)
                BV_LOG_ERROR("File named {0} not found !", filePath.generic_string());

            return std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
        }

    }

    static GLenum findGlslType(const std::string& token)
    {
        if (token == "vertex")
            return GL_VERTEX_SHADER;
        else if (token == "fragment")
            return GL_FRAGMENT_SHADER;

        BV_ASSERT(false, "Token not known!");
        return 0;
    }

    static std::unordered_map<GLenum, std::string> extractShaderSources(const std::filesystem::path& shaderFilePath)
    {
        std::string source = Utils::readFile(shaderFilePath);

        std::unordered_map<GLenum, std::string> shaderSources;

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);

        while (pos != std::string::npos)
        {
            // find the substring of the type
            size_t endLine = source.find_first_of("\r\n", pos);
            BV_ASSERT(endLine != std::string::npos, "Shader syntaxe error!");
            size_t count = endLine - (pos + typeTokenLength + 1);
            std::string type = source.substr(pos + typeTokenLength + 1, count);
            GLenum openGLType = findGlslType(type); // convert to opengl enum type

            // Extract the source code
            pos = endLine + 1;
            size_t endShaderSource = source.find(typeToken, pos);
            size_t shaderSourceLength = endShaderSource - pos;
            std::string subSource = source.substr(pos, shaderSourceLength);
            shaderSources[openGLType] = subSource;

            // Goes to the next token
            pos = source.find(typeToken, shaderSourceLength + 1);
        }

        return shaderSources;
    }

    static uint32_t compileShader(const std::string& shaderContent, GLenum shaderType)
    {
        int32_t success;
        char log[128];

        int32_t shaderID;
        shaderID = glCreateShader(shaderType);

        const char* rawShaderContent = shaderContent.c_str();
        glShaderSource(shaderID, 1, &rawShaderContent, nullptr);

        glCompileShader(shaderID);

        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shaderID, 128, nullptr, log);
            std::string shaderTypeStr;
            switch (shaderType)
            {
                case GL_VERTEX_SHADER:   
                    shaderTypeStr = "Vertex";
                    break;
                case GL_FRAGMENT_SHADER: 
                    shaderTypeStr = "Fragment"; 
                    break;
                default:
                    shaderTypeStr = "[Unknown]";
                    break;
            }
            BV_LOG_WARN("{} shader compilation failed !", shaderTypeStr);
        }

        return shaderID;
    }

    static uint32_t compileAndLink(const std::unordered_map<GLenum, std::string>& shaderSources)
    {
        // We have only 2 shader possible (vertex shader ad fragment shader)
        std::array<uint32_t, 2> shaderIDs;

        uint32_t programID = 0;
        programID = glCreateProgram();

        uint8_t i = 0;
        for (const auto& [shaderType, shaderSource] : shaderSources)
        {
            uint32_t shaderID = compileShader(shaderSource, shaderType);
            shaderIDs[i] = shaderID;
            glAttachShader(programID, shaderID);
            i++;
        }

        glLinkProgram(programID);

        // DEBUG
        {
            int32_t success;
            char log[128];
            glGetProgramiv(programID, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(programID, 128, nullptr, log);
                BV_LOG_WARN("Shader program linking failed !");
            }
        }

        for (uint8_t i = 0; i < shaderSources.size(); i++)
        {
            auto shaderID = shaderIDs[i];
            glDeleteShader(shaderID);
        }

        return programID;
    }

    Shader::Shader(const std::filesystem::path& filePath)
    {
        invalidate(filePath);
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_shaderProgramID);
    }

    void Shader::bind() const
    {
        glUseProgram(m_shaderProgramID);
    }

    void Shader::unbind() const
    {
        glUseProgram(0);
    }

    void Shader::setUniforms(const UniformMap& uniforms)
    {
        for (const auto& [name, uniform] : uniforms)
        {
            switch (uniform.Type)
            {
                case BasicTypes::Bool:
                    setBool(name, uniform.Data);
                    continue;
                case BasicTypes::Float3:
                    setFloat3(name, uniform.Data);
                    continue;
                case BasicTypes::Mat4:
                    setMat4(name, uniform.Data);
                    continue;
            }

            BV_ASSERT(false, "Uniform type not implemented");
        }
    }

    void Shader::invalidate(const std::filesystem::path& filePath)
    {
        if (m_shaderProgramID)
        {
            glDeleteProgram(m_shaderProgramID);
        }

        auto shaderSources = extractShaderSources(filePath);
        m_shaderProgramID = compileAndLink(shaderSources);

        std::string path = filePath.generic_string();
        size_t lastSlash = path.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        size_t lastPoint = path.rfind('.');
        lastPoint = lastPoint == std::string::npos ? path.size() : lastPoint;
        size_t count = lastPoint - lastSlash;
        m_name = path.substr(lastSlash, count);
    }

    void Shader::setBool(const std::string& name, const void* data)
    {
        uint32_t loc = glGetUniformLocation(m_shaderProgramID, name.c_str());
        CHECK_GL_ERROR();
        bool value = *((bool*)data);
        glUniform1i(loc, value);
    }

    void Shader::setFloat3(const std::string& name, const void* data)
    {
        // set uniform color
        int32_t loc = glGetUniformLocation(m_shaderProgramID, name.c_str());
        glUniform3fv(loc, 1, (float*)data);
    }

    void Shader::setMat4(const std::string& name, const void* data)
    {
        int32_t loc = glGetUniformLocation(m_shaderProgramID, name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)data);
    }
}