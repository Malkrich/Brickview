#include "Pch.h"
#include "OpenGLError.h"

#include "Core/Log.h"

#include <glad/glad.h>

namespace Brickview
{

    void checkGLError(const char *file, int line)
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
