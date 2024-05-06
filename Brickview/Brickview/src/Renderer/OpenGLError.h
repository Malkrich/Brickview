#pragma once

#ifdef BV_DEBUG
	#define CHECK_GL_ERROR() Brickview::checkGLError(__FILE__,__LINE__)
#else
	#define CHECK_GL_ERROR()
#endif

namespace Brickview
{

	//https://blog.nobel-joergensen.com/2013/01/29/debugging-opengl-using-glgeterror/
	void checkGLError(const char *file, int32_t line);

}
