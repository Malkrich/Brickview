#pragma once

#ifdef BV_DEBUG
	#define CHECK_GL_ERROR()             Brickview::checkGLError(__FILE__,__LINE__)
	#define CHECK_GLSL_COMPILE_ERROR(id) Brickview::checkGlslError(id, GL_COMPILE_STATUS)
	#define CHECK_GLSL_LINK_ERROR(id)    Brickview::checkGlslError(id, GL_LINK_STATUS)
#else
	#define CHECK_GL_ERROR()
	#define CHECK_GLSL_COMPILE_ERROR(id)
	#define CHECK_GLSL_LINK_ERROR(id)
#endif

namespace Brickview
{

    void checkGlslError(uint32_t shaderID, uint32_t checkType);

	//https://blog.nobel-joergensen.com/2013/01/29/debugging-opengl-using-glgeterror/
	void checkGLError(const char *file, int32_t line);

}
