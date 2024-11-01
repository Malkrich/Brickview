#include "Pch.h"
#include "OpenGLError.h"

#include <glad/glad.h>

namespace Brickview
{

    void checkGlslError(uint32_t id, uint32_t checkType)
    {
        GLint success = 0;
        std::string errorType;
        std::vector<char> log;
        
        switch (checkType)
        {
            case GL_COMPILE_STATUS:
            {
                errorType = "compilation";
                glGetShaderiv(id, checkType, &success);
                GLint shaderInfoLogLength = 0;
                glGetShaderiv(id, GL_INFO_LOG_LENGTH, &shaderInfoLogLength);
                log.resize(shaderInfoLogLength);
                glGetShaderInfoLog(id, shaderInfoLogLength, &shaderInfoLogLength, log.data());
                break;
            }
            case GL_LINK_STATUS:
            {
                errorType = "linking";
                glGetProgramiv(id, checkType, &success);
                GLint shaderInfoLogLength = 0;
                glGetProgramiv(id, GL_INFO_LOG_LENGTH, &shaderInfoLogLength);
                log.resize(shaderInfoLogLength);
                glGetProgramInfoLog(id, shaderInfoLogLength, &shaderInfoLogLength, log.data());
                break;
            }
        }

        if (success)
        {
            BV_LOG_INFO("Shader {} success!", errorType);
            return;
        }

        BV_LOG_ERROR("Shader {} failed !", errorType);

        if (log.empty())
            return;

        BV_LOG_ERROR("{}", log.data());
    }

    void checkGLError(const char *file, int32_t line)
    {
        GLenum err (glGetError());

        while(err != GL_NO_ERROR)
        {
            std::string error;
            switch(err)
            {
                case GL_INVALID_OPERATION:              error = "INVALID_OPERATION";              break;
                case GL_INVALID_ENUM:                   error = "INVALID_ENUM";                   break;
                case GL_INVALID_VALUE:                  error = "INVALID_VALUE";                  break;
                case GL_OUT_OF_MEMORY:                  error = "OUT_OF_MEMORY";                  break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
                case GL_STACK_UNDERFLOW:                error = "GL_STACK_UNDERFLOW";             break;
                case GL_STACK_OVERFLOW:                 error = "GL_STACK_OVERFLOW";              break;
            }
            BV_LOG_ERROR("GL_{} - {} : {}", error.c_str(), file, line);
            err = glGetError();
        }
    }

}
