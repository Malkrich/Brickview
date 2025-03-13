#include "Pch.h"
#include "OpenGLError.h"

namespace Brickview
{

    void checkGLError(const char* file, int32_t line)
    {
        GLenum err(glGetError());

        while (err != GL_NO_ERROR)
        {
            std::string error;
            switch (err)
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

    static const char* openGLErrorSourceToStringSource(GLenum source)
    {
        switch (source)
        {
            case GL_DEBUG_SOURCE_API:             return "OpenGL API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "Window system";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY:     return "Third party";
            case GL_DEBUG_SOURCE_APPLICATION:     return "Application";
            case GL_DEBUG_SOURCE_OTHER:           return "Other";
        }

        BV_ASSERT(false, "unknown OpenGL error source enum!");
        return "";
    }

    static const char* openGLErrorTypeToStringType(GLenum type)
    {
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR:               return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated behavior";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined behavior";
            case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
            case GL_DEBUG_TYPE_MARKER:              return "Marker";
            case GL_DEBUG_TYPE_PUSH_GROUP:          return "Push group";
            case GL_DEBUG_TYPE_POP_GROUP:           return "Pop group";
            case GL_DEBUG_TYPE_OTHER:               return "Other";
        }

        BV_ASSERT(false, "unknown OpenGL error type enum!");
        return "";
    }

    #define BV_OPENGL_MESSAGE_CALLBACK_LOG "[OpenGL Error Message]:\n" \
                                           "    - Source: {}\n" \
                                           "    - Type: {}\n" \
                                           "    - Message: {}\n" \

    void openGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
    {
        const char* sourceStr = openGLErrorSourceToStringSource(source);
        const char* typeStr = openGLErrorTypeToStringType(type);

        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:
                BV_LOG_ERROR(BV_OPENGL_MESSAGE_CALLBACK_LOG, sourceStr, typeStr, message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                BV_LOG_WARN(BV_OPENGL_MESSAGE_CALLBACK_LOG, sourceStr, typeStr, message);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                BV_LOG_INFO(BV_OPENGL_MESSAGE_CALLBACK_LOG, sourceStr, typeStr, message);
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                BV_LOG_TRACE(BV_OPENGL_MESSAGE_CALLBACK_LOG, sourceStr, typeStr, message);
                break;
        }

    }

}
